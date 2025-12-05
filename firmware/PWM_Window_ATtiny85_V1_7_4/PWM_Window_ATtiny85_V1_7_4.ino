/*
 * ═══════════════════════════════════════════════════════════════
 * CIRCUIT PWM µC V1.7.4 - ATtiny85
 * ═══════════════════════════════════════════════════════════════
 * 
 * Changements vs 1.7.3:
 *   - volatile outputState : Barrière mémoire explicite (défensif)
 *   - cli() avant SREG restore : Atomicité complète (défensif)
 *   - Documentation BOD : Explication choix fuses
 *   - #include <Arduino.h> : Compatibilité PlatformIO/CLI
 *   - #warning F_CPU : Avertissement si != 8MHz
 * 
 * Changements vs 1.7.2:
 *   - DIDR0 |= : Préserve autres bits ADC si ajout futur
 *   - pinMode(PWM_IN, INPUT) : Configuration explicite entrée
 *   - Protection numSamples == 0 : Évite division par zéro
 *   - Macro MUX_BANDGAP : Lisibilité sélection ADC
 *   - Prescaler F_CPU < 400kHz : Couverture edge case ADC clock
 *
 * Changements vs 1.7.1:
 *   - SREG RESTORE: Sauvegarde/restauration état IRQ dans sleep
 *
 * Changements vs 1.7.0:
 *   - RÉVEIL VCC: WDT configuré AVANT boucle sécurité
 *   - AUTO-RECOVERY: Sortie automatique safe mode si VCC > 4.7V
 * 
 * Changements vs 1.6.3:
 *   - SLEEP ATOMIQUE: cli() ajouté (conforme datasheet p.39)
 *   - SUPPRESSION delay() ACTIF: sleep systématique (économie 2,5mA)
 *   - VCC MONITORING ROBUSTE: Gestion ADEN, prescaler adaptatif F_CPU
 *   - CORRECTION BUG WDT: WDP2 seul = 0.25s, corrigé pour 1s réel
 *   - PROTECTION DIV/0: ADC=0 retourne erreur
 * 
 * Historique:
 *   V1.7.4: volatile, cli() atomique, doc BOD, Arduino.h (hardening final)
 *   V1.7.3: Corrections mineures audit (DIDR0, pinMode, edge cases)
 *   V1.7.2: SREG restore (programmation défensive)
 *   V1.7.1: Réveil périodique VCC, auto-recovery cold-crank
 *   V1.7.0: Sleep atomique, VCC monitoring robuste, correction WDT
 *   V1.6.3: Hystérésis symétrique ±520mV
 * 
 * ═══════════════════════════════════════════════════════════════
 */

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <util/delay.h>

#if defined(F_CPU) && F_CPU != 8000000UL
  #warning "F_CPU != 8MHz: Verifier prescaler ADC et timings. Code optimise pour 8MHz."
#endif

#ifndef F_CPU
  #warning "F_CPU non defini: Utilisation prescaler ADC conservateur (128)."
#endif

#define FW_VERSION   "1.7.4"
#define FW_DATE      "2025-11-28"
#define FW_AUTHOR    "mmmprod"
#define HW_REVISION  "V1.10"

#define PWM_IN      A1
#define OUT_CTRL    0

#define MUX_BANDGAP     0b1100
#define VBG_MV          1100

#define SEUIL_BAS   219
#define SEUIL_HAUT  531
#define HYSTERESIS  20

#define SEUIL_BAS_ON      (SEUIL_BAS + HYSTERESIS)
#define SEUIL_BAS_LIMITE  (SEUIL_BAS - HYSTERESIS)
#define SEUIL_HAUT_LIMITE (SEUIL_HAUT - HYSTERESIS)
#define SEUIL_HAUT_ON     (SEUIL_HAUT + HYSTERESIS)

#define NB_SAMPLES_ACTIF  8
#define NB_SAMPLES_VEILLE 3
#define DELAY_SAMPLE      5

#define VCC_MIN_MV       4500
#define VCC_RECOVERY_MV  4700

volatile bool outputState = false;

ISR(WDT_vect) {
}

void configureWDT() {
  cli();
  MCUSR &= ~(1 << WDRF);
  WDTCR = (1 << WDCE) | (1 << WDE);
  WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1);
  sei();
}

uint16_t readVCCmV() {
  uint8_t admux_save = ADMUX;
  uint8_t adcsra_save = ADCSRA;
  bool adc_was_off = !(adcsra_save & (1 << ADEN));

  if (adc_was_off) {
    uint8_t prescaler_bits = 0;
    #if defined(F_CPU)
      #if F_CPU >= 8000000UL
        prescaler_bits = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
      #elif F_CPU >= 4000000UL
        prescaler_bits = (1<<ADPS2)|(1<<ADPS1);
      #elif F_CPU >= 2000000UL
        prescaler_bits = (1<<ADPS2)|(1<<ADPS0);
      #elif F_CPU >= 1000000UL
        prescaler_bits = (1<<ADPS1)|(1<<ADPS0);
      #elif F_CPU >= 400000UL
        prescaler_bits = (1<<ADPS1);
      #else
        prescaler_bits = (1<<ADPS0);
      #endif
    #else
      prescaler_bits = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    #endif

    const uint8_t ADPS_MASK = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    ADCSRA = (adcsra_save & ~ADPS_MASK) | prescaler_bits | (1 << ADEN);
    _delay_ms(1);
  }

  ADMUX = (0 << REFS0) | MUX_BANDGAP;
  _delay_ms(2);

  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));

  uint16_t adc = ADC;

  ADMUX = admux_save;
  ADCSRA = adcsra_save;

  if (adc == 0) {
    return 0;
  }

  return (uint16_t)((VBG_MV * 1023UL) / adc);
}

uint16_t readADC() {
  return analogRead(PWM_IN);
}

uint16_t readFilteredADC(uint8_t numSamples) {
  if (numSamples == 0) {
    return readADC();
  }
  
  uint32_t sum = 0;
  uint16_t minVal = 1023;
  uint16_t maxVal = 0;
  
  for (uint8_t i = 0; i < numSamples; i++) {
    uint16_t val = readADC();
    sum += val;
    
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    
    if (i < numSamples - 1) {
      delay(DELAY_SAMPLE);
    }
  }
  
  if (numSamples >= 3) {
    return (uint16_t)((sum - minVal - maxVal) / (numSamples - 2));
  }
  else {
    return (uint16_t)(sum / numSamples);
  }
}

void updateOutputState(uint16_t adcValue) {
  if (!outputState) {
    if (adcValue >= SEUIL_BAS_ON && adcValue <= SEUIL_HAUT_LIMITE) {
      outputState = true;
      digitalWrite(OUT_CTRL, HIGH);
    }
  }
  else {
    if (adcValue < SEUIL_BAS_LIMITE || adcValue > SEUIL_HAUT_ON) {
      outputState = false;
      digitalWrite(OUT_CTRL, LOW);
    }
  }
}

void enterSleepAtomic() {
  uint8_t sreg_save = SREG;
  cli();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  cli();
  SREG = sreg_save;
}

void setup() {
  pinMode(OUT_CTRL, OUTPUT);
  digitalWrite(OUT_CTRL, LOW);
  
  pinMode(PWM_IN, INPUT);
  
  analogReference(DEFAULT);
  DIDR0 |= (1 << ADC1D);
  
  configureWDT();
  
  uint16_t vcc = readVCCmV();
  
  if (vcc != 0 && vcc < VCC_MIN_MV) {
    delay(100);
    vcc = readVCCmV();
    
    if (vcc != 0 && vcc < VCC_MIN_MV) {
      digitalWrite(OUT_CTRL, LOW);
      
      while (1) {
        enterSleepAtomic();
        
        vcc = readVCCmV();
        
        if (vcc == 0 || vcc < VCC_RECOVERY_MV) {
          continue;
        }
        
        break;
      }
    }
  }
  
  delay(50);
}

void loop() {
  uint8_t samples = outputState ? NB_SAMPLES_ACTIF : NB_SAMPLES_VEILLE;
  uint16_t adc = readFilteredADC(samples);
  
  updateOutputState(adc);
  
  enterSleepAtomic();
}
