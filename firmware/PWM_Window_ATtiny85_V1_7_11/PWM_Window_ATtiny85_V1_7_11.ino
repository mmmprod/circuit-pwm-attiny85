/*
 * ═══════════════════════════════════════════════════════════════
 * CIRCUIT PWM µC V1.7.11 - ATtiny85
 * ═══════════════════════════════════════════════════════════════
 * 
 * Changements vs 1.7.10:
 *   - WDT 250ms: Réactivité ×4 (latence 290ms vs 1040ms)
 *   - CORRECTION DOC: hfuse EESAVE=1 = NON préservée
 *   - Commentaires latence mis à jour
 * 
 * Changements vs 1.7.7:
 *   - PROTECTION OVERFLOW VCC : Si adc < 17 → retourne 0 (erreur)
 *   - TIMEOUT BOUCLE ADC : Évite blocage si ADC hardware défaillant
 * 
 * Changements vs 1.7.6:
 *   - DÉTECTION ADC HORS PLAGE : Fail-safe si diviseur défaillant
 *   - ADC < 50 ou > 950 → sortie OFF (défaut hardware)
 * 
 * Changements vs 1.7.5:
 *   - DUMMY READ après réveil : Stabilisation ADC (datasheet p.146)
 *   - Première conversion ignorée après sleep (précision améliorée)
 * 
 * Changements vs 1.7.4:
 *   - ADC DÉSACTIVÉ PENDANT SLEEP : Économie ~260µA (datasheet p.151)
 *   - Sauvegarde/restauration ADCSRA dans enterSleepAtomic()
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
 *   V1.7.11: WDT 250ms (réactivité ×4), correction doc hfuse
 *   V1.7.10: Timeout readADC() (cohérence avec readVCCmV) - Audit Gemini
 *   V1.7.9: HW_REVISION V1.15 (mise à jour documentaire)
 *   V1.7.8: Protections défensives (overflow VCC, timeout ADC)
 *   V1.7.7: Détection ADC hors plage (fail-safe diviseur défaillant)
 *   V1.7.6: Dummy read après réveil (stabilisation ADC)
 *   V1.7.5: Désactivation ADC pendant sleep (économie 260µA)
 *   V1.7.4: volatile, cli() atomique, doc BOD, Arduino.h (hardening final)
 *   V1.7.3: Corrections mineures audit (DIDR0, pinMode, edge cases)
 *   V1.7.2: SREG restore (programmation défensive)
 *   V1.7.1: Réveil périodique VCC, auto-recovery cold-crank
 *   V1.7.0: Sleep atomique, VCC monitoring robuste, correction WDT
 *   V1.6.3: Hystérésis symétrique ±520mV
 * 
 * ═══════════════════════════════════════════════════════════════
 */

// ═══════════════════════════════════════════════════════════════
// INCLUDES
// ═══════════════════════════════════════════════════════════════
// 
// Arduino.h: Requis explicitement pour compatibilité:
//   - PlatformIO
//   - avr-gcc CLI
//   - Autres toolchains non-Arduino IDE
// 
// Note: Arduino IDE l'inclut automatiquement, mais l'expliciter
//       garantit la portabilité du code.
//
// ═══════════════════════════════════════════════════════════════

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <util/delay.h>

// ═══════════════════════════════════════════════════════════════
// VÉRIFICATION F_CPU
// ═══════════════════════════════════════════════════════════════
// 
// Ce code est optimisé pour ATtiny85 @ 8MHz (oscillateur interne).
// Les prescalers ADC et timings sont calculés pour cette fréquence.
// 
// Si F_CPU différent, le code reste fonctionnel mais:
//   - Prescaler ADC adapté automatiquement
//   - Timings delay() ajustés par le core
//   - NB_SAMPLES et DELAY_SAMPLE peuvent nécessiter ajustement
//
// ═══════════════════════════════════════════════════════════════

#if defined(F_CPU) && F_CPU != 8000000UL
  #warning "F_CPU != 8MHz: Verifier prescaler ADC et timings. Code optimise pour 8MHz."
#endif

#ifndef F_CPU
  #warning "F_CPU non defini: Utilisation prescaler ADC conservateur (128)."
#endif

// ═══════════════════════════════════════════════════════════════
// INFORMATIONS VERSION
// ═══════════════════════════════════════════════════════════════

#define FW_VERSION   "1.7.11"
#define FW_DATE      "2025-12-19"
#define FW_AUTHOR    "mmmprod"
#define HW_REVISION  "V1.18"

// ═══════════════════════════════════════════════════════════════
// CONFIGURATION HARDWARE
// ═══════════════════════════════════════════════════════════════

#define PWM_IN      A1          // ADC1 = PB2 = pin7
#define OUT_CTRL    0           // PB0 = pin5

// ═══════════════════════════════════════════════════════════════
// CONFIGURATION ADC - REGISTRES
// ═══════════════════════════════════════════════════════════════
// 
// ATtiny85 Datasheet Table 17-4 (p.134):
//   MUX[3:0] = 0b1100 → Vbg (Bandgap 1.1V interne)
//   REFS[2:0] = 0b000 → VCC comme référence
//
// ═══════════════════════════════════════════════════════════════

#define MUX_BANDGAP     0b1100  // Sélection entrée bandgap 1.1V
#define VBG_MV          1100    // Tension bandgap nominale (1.0-1.2V selon exemplaire)

// ═══════════════════════════════════════════════════════════════
// SEUILS ADC - DIVISEUR 33k/20k (k=0,377)
// ═══════════════════════════════════════════════════════════════
// 
// Calculs validés Circuit V1.14:
//   k = 20k/(33k+20k) = 0,37736
//   ΔV_PWM/count = 5V / 0,377 / 1023 = 12,95 mV/count
//
// Centres de fenêtre:
//   SEUIL_BAS = 219  → 2,84V PWM (limite basse)
//   SEUIL_HAUT = 531 → 6,88V PWM (limite haute)
//
// Hystérésis:
//   HYSTERESIS = 20  → ~260mV ADC = ~520mV PWM (anti-rebond)
// 
// ═══════════════════════════════════════════════════════════════

#define SEUIL_BAS         219   // Centre bas fenêtre (2,84V PWM)
#define SEUIL_HAUT        531   // Centre haut fenêtre (6,88V PWM)
#define HYSTERESIS        20    // ±260mV ADC = ±520mV PWM

// Seuils calculés avec hystérésis
#define SEUIL_BAS_LIMITE  (SEUIL_BAS - HYSTERESIS)   // 199 = 2,58V
#define SEUIL_BAS_ON      (SEUIL_BAS + HYSTERESIS)   // 239 = 3,10V
#define SEUIL_HAUT_LIMITE (SEUIL_HAUT - HYSTERESIS)  // 511 = 6,61V
#define SEUIL_HAUT_ON     (SEUIL_HAUT + HYSTERESIS)  // 551 = 7,14V

// ═══════════════════════════════════════════════════════════════
// SEUILS ADC PLAGE VALIDE (V1.7.7)
// ═══════════════════════════════════════════════════════════════
// 
// Détection défaut hardware diviseur R1/R2:
//   ADC < 50  → Probablement R2 court-circuit GND
//   ADC > 950 → Probablement R1 court-circuit +5V
// 
// Plage normale avec signal PWM 0-12V:
//   PWM 0V  → ADC ~0 (mais bruit → ~10-30)
//   PWM 12V → ADC ~925 (après clamp 5V diviseur)
// 
// Marges de sécurité:
//   50 = ~0,24V ADC = ~0,65V PWM (bruit + offset)
//   950 = ~4,64V ADC = ~12,3V PWM (proche limite)
//
// ═══════════════════════════════════════════════════════════════

#define ADC_MIN_VALID     50    // ADC minimum valide
#define ADC_MAX_VALID     950   // ADC maximum valide

// ═══════════════════════════════════════════════════════════════
// CONFIGURATION ÉCHANTILLONNAGE
// ═══════════════════════════════════════════════════════════════

#define NB_SAMPLES_VEILLE  8    // Échantillons en mode veille
#define NB_SAMPLES_ACTIF   6    // Échantillons en mode actif
#define DELAY_SAMPLE       5    // Délai entre échantillons (ms)

// ═══════════════════════════════════════════════════════════════
// CONFIGURATION SÉCURITÉ VCC
// ═══════════════════════════════════════════════════════════════

#define VCC_MIN_MV       4500   // 4,5V minimum pour fonctionnement
#define VCC_RECOVERY_MV  4700   // 4,7V pour sortie safe mode (hystérésis)

// ═══════════════════════════════════════════════════════════════
// VARIABLES GLOBALES
// ═══════════════════════════════════════════════════════════════
// 
// volatile: Barrière mémoire explicite (programmation défensive)
// Garantit que le compilateur ne cache pas cette variable dans
// un registre entre les itérations de loop(), même si l'ISR est vide.
//
// Note technique: Dans notre code actuel, l'ISR WDT_vect est vide
// donc volatile n'est pas strictement requis. Mais si du code est
// ajouté à l'ISR dans le futur, volatile sera déjà en place.
//
// ═══════════════════════════════════════════════════════════════

volatile bool outputState = false;  // État sortie: false=OFF, true=ON

// ═══════════════════════════════════════════════════════════════
// ISR WATCHDOG (réveil sleep)
// ═══════════════════════════════════════════════════════════════
// 
// Note: ISR vide intentionnellement.
// Le WDT sert uniquement à réveiller le µC depuis POWER_DOWN.
// Si code ajouté ici à l'avenir: outputState est déjà volatile.
//
// ═══════════════════════════════════════════════════════════════

ISR(WDT_vect) {
  // Interruption watchdog pour réveil sleep
  // Pas d'action nécessaire, juste sortir du sleep
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Configuration Watchdog
// ═══════════════════════════════════════════════════════════════
//
// @brief Configure le Watchdog Timer en mode interrupt-only (250ms)
// @note  Doit être appelée avant toute utilisation de enterSleepAtomic()
// @note  Mode WDIE = interrupt seulement (pas de reset système)
// 
// Table WDP ATtiny85 (datasheet p.46):
//   WDP3 WDP2 WDP1 WDP0 | Timeout
//    0    0    0    0   | 16ms
//    0    0    0    1   | 32ms
//    0    0    1    0   | 64ms
//    0    0    1    1   | 0.125s
//    0    1    0    0   | 0.25s  ← V1.7.11: Utilisé (réactivité ×4)
//    0    1    1    0   | 1s     ← V1.7.10 et avant
//    1    0    0    1   | 8s
// 
// V1.7.11: Passage de 1s à 250ms pour réactivité accrue
//   - Latence pire cas: 1040ms → 290ms (amélioration ×3,6)
//   - Consommation: Impact négligeable (réveil 4× plus fréquent
//     mais phase active très courte ~40ms)
//   - Bénéfice: Meilleure réponse aux changements PWM rapides
// 
// ═══════════════════════════════════════════════════════════════

void configureWDT() {
  cli();
  MCUSR &= ~(1 << WDRF);             // Clear watchdog reset flag
  WDTCR = (1 << WDCE) | (1 << WDE);  // Enable watchdog change
  // V1.7.11: WDP2 seul = 250ms (au lieu de WDP2|WDP1 = 1s)
  WDTCR = (1 << WDIE) | (1 << WDP2); // 250ms, interrupt mode
  sei();
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Lecture VCC via référence bandgap interne
// ═══════════════════════════════════════════════════════════════
//
// @brief  Mesure la tension VCC en utilisant la référence bandgap 1.1V
// @return VCC en millivolts, ou 0 si erreur (division par zéro)
//
// @note   Comportement adaptatif selon état ADC:
//         - Si ADEN=1: garde prescaler existant (respect Arduino core)
//         - Si ADEN=0: active avec prescaler optimal selon F_CPU
//
// @note   Sauvegarde/restaure ADMUX et ADCSRA → pas de side-effects
//
// @note   Tolérance bandgap: ±10% selon exemplaire/température
//         VBG_MV peut être calibré en EEPROM si précision requise
//
// Prescalers selon F_CPU (ADC clock cible: 50-200 kHz):
//   8 MHz   → /128 → 62,5 kHz ✓
//   4 MHz   → /64  → 62,5 kHz ✓
//   2 MHz   → /32  → 62,5 kHz ✓
//   1 MHz   → /8   → 125 kHz  ✓
//   <400kHz → /4   → ~100 kHz ✓ (edge case)
//
// ═══════════════════════════════════════════════════════════════

uint16_t readVCCmV() {
  // Sauvegarder registres ADMUX et ADCSRA
  uint8_t admux_save = ADMUX;
  uint8_t adcsra_save = ADCSRA;

  // Détecter si ADC était désactivé
  bool adc_was_off = !(adcsra_save & (1 << ADEN));

  if (adc_was_off) {
    // ADC désactivé → activer avec prescaler adapté à F_CPU
    uint8_t prescaler_bits = 0;

    // Sélection prescaler pour ADC clock ~50-200 kHz
    // ADPS: 001=2, 010=4, 011=8, 100=16, 101=32, 110=64, 111=128
    #if defined(F_CPU)
      #if F_CPU >= 8000000UL
        // 8 MHz → prescaler 128 → ADC clk = 62,5 kHz
        prescaler_bits = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
      #elif F_CPU >= 4000000UL
        // 4 MHz → prescaler 64 → 62,5 kHz
        prescaler_bits = (1<<ADPS2)|(1<<ADPS1);
      #elif F_CPU >= 2000000UL
        // 2 MHz → prescaler 32 → 62,5 kHz
        prescaler_bits = (1<<ADPS2)|(1<<ADPS0);
      #elif F_CPU >= 1000000UL
        // 1 MHz → prescaler 8 → 125 kHz
        prescaler_bits = (1<<ADPS1)|(1<<ADPS0);
      #elif F_CPU >= 400000UL
        // 400-999 kHz → prescaler 4 → 100-250 kHz
        prescaler_bits = (1<<ADPS1);
      #else
        // <400 kHz → prescaler 2 → ~200 kHz
        prescaler_bits = (1<<ADPS0);
      #endif
    #else
      // F_CPU non défini → fallback conservateur (prescaler 128)
      prescaler_bits = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    #endif

    // Configurer ADCSRA: préserver autres bits, définir ADEN et prescaler
    const uint8_t ADPS_MASK = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    ADCSRA = (adcsra_save & ~ADPS_MASK) | prescaler_bits | (1 << ADEN);
    
    // Attente stabilisation ADC clock
    _delay_ms(1);
  }

  // Configurer ADMUX: référence VCC, entrée bandgap (1.1V)
  // ATtiny85: REFS=0 (VCC), MUX=0b1100 (bandgap)
  ADMUX = (0 << REFS0) | MUX_BANDGAP;

  // Délai stabilisation bandgap (~70µs typ, 2ms pour marge)
  _delay_ms(2);

  // ✓ V1.7.8: Timeout boucle ADC (protection hardware défaillant)
  // Conversion ADC = 13 cycles @ 62,5 kHz = 208µs typ
  // Timeout 1000 itérations = marge énorme (~10ms worst case)
  // Si timeout → ADC hardware mort → retourne 0 (erreur)
  ADCSRA |= (1 << ADSC);
  uint16_t timeout = 1000;
  while ((ADCSRA & (1 << ADSC)) && --timeout);
  
  // Vérifier si timeout atteint (ADC hardware défaillant)
  if (timeout == 0) {
    ADMUX = admux_save;
    ADCSRA = adcsra_save;
    return 0;  // Erreur: ADC bloqué
  }

  // Lecture résultat 10 bits
  uint16_t adc = ADC;

  // Restaurer registres précédents
  ADMUX = admux_save;
  ADCSRA = adcsra_save;

  // Protection division par zéro
  // ADC=0 impossible en fonctionnement normal → erreur hardware
  if (adc == 0) {
    return 0;  // Signale erreur (appelant doit gérer)
  }

  // ✓ V1.7.8: Protection overflow uint32→uint16
  // Si adc < 17 → résultat > 65535 → overflow silencieux
  // Cas pathologique: adc < 17 implique VCC > 65V (impossible)
  // Mais protection défensive pour robustesse
  uint32_t result = (VBG_MV * 1023UL) / adc;
  if (result > 65535) {
    return 0;  // Erreur: VCC hors plage (cas pathologique)
  }
  
  return (uint16_t)result;
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Lecture ADC simple
// ═══════════════════════════════════════════════════════════════

uint16_t readADC() {
  // ✓ V1.7.10: Lecture ADC avec timeout (cohérence avec readVCCmV)
  // analogRead() peut bloquer si ADC hardware défaillant
  
  // S'assurer ADC activé (peut être désactivé après sleep)
  if (!(ADCSRA & (1 << ADEN))) {
    // ADC désactivé -> réactiver avec prescaler 128 (F_CPU=8MHz -> ADC_clk=62.5kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    _delay_us(200);  // Stabilisation ADC clock
  }
  
  // Configurer ADMUX: référence VCC (REFS=0), canal ADC1 (MUX=1)
  // PWM_IN = A1 = ADC1 = PB2 sur ATtiny85
  ADMUX = (0 << REFS0) | 1;
  
  // Démarrer conversion
  ADCSRA |= (1 << ADSC);
  
  // Timeout protection (même logique que readVCCmV)
  // Conversion ADC = 13 cycles @ 62,5 kHz = 208us typ
  // Timeout 1000 itérations = marge énorme (~10ms worst case)
  uint16_t timeout = 1000;
  while ((ADCSRA & (1 << ADSC)) && --timeout);
  
  // Si timeout -> ADC hardware défaillant
  // Retourne ADC_MAX_VALID + 1 = 951 -> déclenche "défaut hardware" dans loop()
  if (timeout == 0) {
    return ADC_MAX_VALID + 1;
  }
  
  return ADC;
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Lecture ADC filtrée
// ═══════════════════════════════════════════════════════════════
// Moyenne en rejetant min et max (filtre bruit sans tri O(n²))
// Note: delay() ici est NÉCESSAIRE (espacement échantillons)
// ═══════════════════════════════════════════════════════════════

uint16_t readFilteredADC(uint8_t numSamples) {
  // Protection division par zéro
  if (numSamples == 0) {
    return readADC();  // Fallback lecture simple
  }
  
  uint32_t sum = 0;
  uint16_t minVal = 1023;
  uint16_t maxVal = 0;
  
  // Acquisition N échantillons espacés de DELAY_SAMPLE ms
  for (uint8_t i = 0; i < numSamples; i++) {
    uint16_t val = readADC();
    sum += val;
    
    // Track min/max
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    
    if (i < numSamples - 1) {
      delay(DELAY_SAMPLE);
    }
  }
  
  // Moyenne en rejetant extrêmes (si ≥3 samples)
  if (numSamples >= 3) {
    return (uint16_t)((sum - minVal - maxVal) / (numSamples - 2));
  }
  else {
    return (uint16_t)(sum / numSamples);
  }
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Mise à jour état sortie
// ═══════════════════════════════════════════════════════════════
// 
// Logique P-MOSFET avec étage inverseur BS170:
//   OUT_CTRL=LOW  → BS170 OFF → GATE_P=+12V (pull-up) → P-MOS OFF
//   OUT_CTRL=HIGH → BS170 ON  → GATE_P=GND            → P-MOS ON
//
// Hystérésis symétrique ±520mV:
//   Zone 199-238: Reste dans état actuel (bande morte basse)
//   Zone 239-511: Fenêtre active (allume si OFF)
//   Zone 512-550: Reste dans état actuel (bande morte haute)
//   <199 ou >551: Hors fenêtre (éteint si ON)
//
// ═══════════════════════════════════════════════════════════════

void updateOutputState(uint16_t adcValue) {
  
  // État OFF → Test activation (entrée dans fenêtre)
  if (!outputState) {
    if (adcValue >= SEUIL_BAS_ON && adcValue <= SEUIL_HAUT_LIMITE) {
      outputState = true;
      digitalWrite(OUT_CTRL, HIGH);   // P-MOSFET ON
    }
  }
  
  // État ON → Test désactivation (sortie de fenêtre)
  else {
    if (adcValue < SEUIL_BAS_LIMITE || adcValue > SEUIL_HAUT_ON) {
      outputState = false;
      digitalWrite(OUT_CTRL, LOW);    // P-MOSFET OFF
    }
  }
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Mode sleep ATOMIQUE avec restauration SREG et ADC OFF
// ═══════════════════════════════════════════════════════════════
//
// @brief  Entre en sleep power-down de manière atomique
// @note   Pattern datasheet ATtiny85 p.39:
//         cli() garantit qu'aucune IRQ n'interrompt entre sei() et sleep_cpu()
// @note   Sauvegarde/restauration SREG pour programmation défensive:
//         Si appelée depuis contexte IRQ désactivées, l'état est préservé
//
// ✓ V1.7.5: Désactivation ADC pendant sleep
//    Datasheet ATtiny85 p.151: "The ADC consumes approximately 260µA 
//    in Active mode. By setting the ADEN bit to zero, the ADC is 
//    turned off and no power is consumed."
//    
//    Impact: ~260µA économisés pendant chaque cycle sleep (~250ms)
//    Conso sleep: ~0,5mA → ~0,24mA (réduction ~50%)
//
// ═══════════════════════════════════════════════════════════════

void enterSleepAtomic() {
  uint8_t sreg_save = SREG;            // Sauvegarde état interruptions
  uint8_t adcsra_save = ADCSRA;        // ✓ V1.7.5: Sauvegarde état ADC
  
  cli();                               // Bloque IRQ (atomicité garantie)
  ADCSRA &= ~(1 << ADEN);              // ✓ V1.7.5: Désactive ADC (économie 260µA)
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();                               // Réactive IRQ (instruction suivante garantie)
  sleep_cpu();                         // Dort ici (atomique)
  sleep_disable();                     // Réveil après WDT
  cli();                               // Bloque IRQ avant restauration
  ADCSRA = adcsra_save;                // ✓ V1.7.5: Restaure ADC
  SREG = sreg_save;                    // Restaure état interruptions
}

// ═══════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════

void setup() {
  // ───────────────────────────────────────────────────────────
  // Configuration I/O (avant tout le reste pour état sûr)
  // ───────────────────────────────────────────────────────────
  pinMode(OUT_CTRL, OUTPUT);
  digitalWrite(OUT_CTRL, LOW);        // État repos sûr: P-MOSFET OFF
  
  pinMode(PWM_IN, INPUT);             // Configuration explicite entrée
  
  // ───────────────────────────────────────────────────────────
  // Configuration ADC
  // ───────────────────────────────────────────────────────────
  analogReference(DEFAULT);           // Référence VCC (5V)
  DIDR0 |= (1 << ADC1D);             // Désactive buffer numérique ADC1
  
  // ───────────────────────────────────────────────────────────
  // Configuration Watchdog AVANT boucle sécurité
  // ───────────────────────────────────────────────────────────
  // Permet réveil périodique pour retester VCC en cas de cold-crank
  // V1.7.11: 250ms (au lieu de 1s) pour réactivité accrue
  // ───────────────────────────────────────────────────────────
  configureWDT();
  
  // ───────────────────────────────────────────────────────────
  // VCC monitoring avec auto-recovery
  // ───────────────────────────────────────────────────────────
  // Boucle de sécurité avec réveil périodique:
  //   - Si VCC < 4.5V: reste en safe mode, sortie OFF
  //   - Réveil toutes les 250ms pour retester VCC (V1.7.11)
  //   - Sortie automatique si VCC > 4.7V (hystérésis)
  // 
  // Cas d'usage: Cold-crank automobile
  //   - Batterie chute à 6V pendant ~2-5s
  //   - LDO peut sortir < 4.5V temporairement
  //   - Après démarrage, batterie remonte rapidement
  //   - Circuit reprend automatiquement
  //
  // Design choice: Pas de timeout sur la boucle safe-mode
  //   - Si ADC défaillant (vcc=0 permanent), reste en safe-mode
  //   - Comportement fail-safe: sortie OFF garantie
  //   - Préférable à un fonctionnement erratique (fail-operational)
  // ───────────────────────────────────────────────────────────
  uint16_t vcc = readVCCmV();
  
  // Première lecture basse → attendre et retester
  if (vcc != 0 && vcc < VCC_MIN_MV) {
    delay(100);                       // Attente transitoire court
    vcc = readVCCmV();
    
    // Toujours bas après 100ms → entrer en safe mode
    if (vcc != 0 && vcc < VCC_MIN_MV) {
      digitalWrite(OUT_CTRL, LOW);    // Force OFF
      
      // Boucle safe mode avec réveil périodique WDT
      while (1) {
        enterSleepAtomic();           // Dort 250ms (WDT configuré V1.7.11)
        
        // Au réveil, retester VCC
        vcc = readVCCmV();
        
        // Si erreur ADC (vcc=0) ou VCC toujours bas → rester en safe mode
        if (vcc == 0 || vcc < VCC_RECOVERY_MV) {
          continue;  // Reste dans la boucle (fail-safe)
        }
        
        // VCC OK (> 4.7V) → sortie safe mode
        break;
      }
    }
  }
  
  // Délai stabilisation finale
  delay(50);
}

// ═══════════════════════════════════════════════════════════════
// LOOP PRINCIPALE
// ═══════════════════════════════════════════════════════════════
// Suppression delay() actif: sleep systématique même en mode ON
// Économie: ~2,5mA (CPU actif 3mA → sleep 0,5mA)
// 
// ✓ V1.7.5: Avec ADC désactivé pendant sleep:
//    Économie additionnelle: ~260µA
//    Total sleep: ~0,24mA (vs ~0,5mA en V1.7.4)
// 
// ✓ V1.7.6: Dummy read après réveil:
//    Datasheet ATtiny85 p.146: "The first ADC conversion result 
//    after switching reference voltage source may be inaccurate"
//    Première conversion ignorée → lectures suivantes précises
// 
// ✓ V1.7.7: Détection ADC hors plage:
//    Si ADC < 50 ou > 950 → défaut hardware diviseur
//    Comportement fail-safe: sortie OFF garantie
//    Scénarios détectés:
//      - R2 court-circuit vers GND → ADC ≈ 0
//      - R1 court-circuit vers +5V → ADC ≈ 1023
//      - Diviseur cassé/dessoudé → ADC erratique
// 
// ✓ V1.7.11: Latence réduite ×4:
//    Sleep WDT: 250ms (vs 1000ms)
//    Latence totale pire cas: ~290ms (vs ~1040ms)
// 
// Latence totale pire cas V1.7.11:
//   Dummy read: ~104µs (1 conversion)
//   Échantillonnage: 8 × 5ms = 40ms
//   Sleep WDT: 250ms
//   Total: ~290ms max (amélioration ×3,6 vs V1.7.10)
// 
// Acceptable pour jauge d'éclairage (changement lent utilisateur)
// ═══════════════════════════════════════════════════════════════

void loop() {
  
  // ───────────────────────────────────────────────────────────
  // ✓ V1.7.6: Dummy read stabilisation ADC après réveil
  // ───────────────────────────────────────────────────────────
  // Après sleep, l'ADC a été désactivé (ADEN=0) puis restauré.
  // La première conversion peut être imprécise (datasheet p.146).
  // On fait une lecture "poubelle" pour stabiliser le circuit.
  // ───────────────────────────────────────────────────────────
  (void)readADC();  // Résultat ignoré (stabilisation)
  
  // ───────────────────────────────────────────────────────────
  // Lecture ADC filtrée
  // ───────────────────────────────────────────────────────────
  uint8_t samples = outputState ? NB_SAMPLES_ACTIF : NB_SAMPLES_VEILLE;
  uint16_t adc = readFilteredADC(samples);
  
  // ───────────────────────────────────────────────────────────
  // ✓ V1.7.7: Détection ADC hors plage (défaut hardware)
  // ───────────────────────────────────────────────────────────
  // Si ADC hors plage normale → diviseur R1/R2 défaillant
  // Comportement fail-safe: sortie OFF, pas de mise à jour état
  // Note: On continue à dormir/réveiller pour retester périodiquement
  // ───────────────────────────────────────────────────────────
  if (adc < ADC_MIN_VALID || adc > ADC_MAX_VALID) {
    // Défaut hardware détecté → fail-safe OFF
    if (outputState) {
      outputState = false;
      digitalWrite(OUT_CTRL, LOW);  // Force OFF immédiat
    }
    // Skip updateOutputState(), aller directement au sleep
    // Au prochain réveil, on reteste (auto-recovery si défaut transitoire)
  }
  else {
    // ───────────────────────────────────────────────────────────
    // Mise à jour état sortie selon fenêtre (ADC valide)
    // ───────────────────────────────────────────────────────────
    updateOutputState(adc);
  }
  
  // ───────────────────────────────────────────────────────────
  // Sleep systématique (ADC désactivé automatiquement V1.7.5)
  // ───────────────────────────────────────────────────────────
  enterSleepAtomic();
}

// ═══════════════════════════════════════════════════════════════
// NOTES DÉVELOPPEUR V1.7.11
// ═══════════════════════════════════════════════════════════════
//
// Amélioration V1.7.11 (audit externe):
//   ✓ WDT 250ms au lieu de 1s
//      Réactivité améliorée ×4 pour détection changements PWM
//      Latence pire cas: 1040ms → 290ms
//      Impact conso: Négligeable (réveil ×4 mais phase active courte)
//
//   ✓ Correction documentation hfuse
//      hfuse = 0xDF → EESAVE=1 signifie EEPROM NON préservée
//      lors du chip erase (pas d'impact, EEPROM non utilisée)
//
// Amélioration V1.7.8 (audit Sonnet - programmation défensive):
//   ✓ Timeout boucle ADC dans readVCCmV()
//      Si ADC hardware bloqué (ADSC jamais clear) → timeout après ~1000 cycles
//      Cas ultra-rare mais protection contre blocage infini
//      Impact: +~20 bytes flash, overhead négligeable
//
//   ✓ Protection overflow VCC (adc < 17)
//      Si adc < 17 → calcul VCC dépasse uint16_t max (65535)
//      Cas pathologique: adc < 17 implique VCC > 65V (circuit détruit)
//      Protection défensive: retourne 0 au lieu d'overflow silencieux
//      Impact: +~10 bytes flash
//
// Amélioration V1.7.7 (audit Sonnet):
//   ✓ Détection ADC hors plage (défaut hardware)
//      Si ADC < 50 ou > 950 → diviseur R1/R2 défaillant
//      Comportement fail-safe: sortie OFF garantie
//      Auto-recovery: reteste à chaque réveil WDT (~250ms)
//      Scénarios couverts:
//        - R2 court-circuit GND → ADC ≈ 0 → détecté
//        - R1 court-circuit +5V → ADC ≈ 1023 → détecté
//        - Diviseur dessoudé/cassé → ADC erratique → détecté
//      Note: Le comportement précédent (sans détection) était déjà 
//            fail-safe (ADC hors fenêtre → sortie OFF), mais silencieux.
//            V1.7.7 rend la détection explicite et documentée.
//
// Amélioration V1.7.6 (audit GPT Codex Max):
//   ✓ Dummy read après réveil sleep
//      Datasheet ATtiny85 p.146: première conversion peut être imprécise
//      après changement de référence ou réactivation ADC
//      Impact: +104µs par cycle (négligeable vs 250ms total)
//      Bénéfice: Lectures ADC plus stables, moins de jitter
//
// Amélioration V1.7.5 (audit externe):
//   ✓ ADC désactivé pendant sleep (ADEN=0)
//      Datasheet ATtiny85 p.151: ADC = 260µA en mode actif
//      Économie: ~260µA par cycle sleep
//      Impact conso totale: ~0,5mA → ~0,24mA en sleep
//
// Hardenings V1.7.4 (audit final):
//   ✓ #include <Arduino.h> : Compatibilité PlatformIO/CLI
//   ✓ #warning F_CPU : Avertissement si != 8MHz
//   ✓ volatile outputState : Barrière mémoire explicite
//   ✓ cli() avant SREG restore : Atomicité complète
//   ✓ Documentation fail-safe : Choix design explicité
//
// Corrections V1.7.3 (audit externe):
//   ✓ DIDR0 |= au lieu de = (préserve autres bits ADC)
//   ✓ pinMode(PWM_IN, INPUT) explicite (hygiène)
//   ✓ Protection numSamples == 0 (évite div/0 futur)
//   ✓ Macro MUX_BANDGAP (lisibilité)
//   ✓ Macro VBG_MV (calibration possible)
//   ✓ Prescaler F_CPU 400kHz-1MHz (edge case)
//
// Améliorations V1.7.2:
//   ✓ SREG sauvegarde/restauration dans enterSleepAtomic()
//
// Améliorations V1.7.1:
//   ✓ WDT configuré AVANT boucle sécurité (réveil périodique)
//   ✓ Auto-recovery: sortie safe mode si VCC > 4.7V
//   ✓ Hystérésis VCC: entrée 4.5V, sortie 4.7V
//
// Améliorations V1.7.0:
//   ✓ Sleep atomique (cli() avant sleep, conforme datasheet)
//   ✓ Suppression delay() actif (économie 2,5mA en mode ON)
//   ✓ VCC monitoring robuste (gestion ADEN, prescaler adaptatif)
//   ✓ Correction bug WDT (0.25s → 1s réel)
//
// Comportement cold-crank:
//   1. Démarrage moteur → VCC chute < 4.5V
//   2. Circuit entre en safe mode (sortie OFF)
//   3. Réveil toutes les 250ms pour retester VCC (V1.7.11)
//   4. Batterie remonte après ~2-5s → VCC > 4.7V
//   5. Circuit sort automatiquement du safe mode
//   6. Fonctionnement normal reprend
//
// Logique P-MOSFET (Circuit V1.18):
//   OUT_CTRL → R11 → BS170 gate
//   BS170 drain → R9 → FQP27P06 gate
//   FQP27P06 source → +12V, drain → R13 → OUT_12V
//   
//   OUT_CTRL=LOW  → BS170 OFF → Gate P-MOS = +12V → P-MOS OFF
//   OUT_CTRL=HIGH → BS170 ON  → Gate P-MOS = GND  → P-MOS ON
//
// Impact flash estimé:
//   V1.6.3: ~950 bytes
//   V1.7.4: ~1180 bytes
//   V1.7.5: ~1200 bytes (+20 bytes vs V1.7.4)
//   V1.7.11: ~1200 bytes (inchangé vs V1.7.5)
//   À MESURER après compilation réelle
//
// Impact consommation V1.7.11:
//   Mode OFF (sleep): ~0,24mA + Iq LDO (~0,4mA) = ~0,64mA total
//   Mode ON (sleep):  ~0,24mA + Iq LDO (~0,4mA) = ~0,64mA total
//   Safe mode: ~0,24mA (sleep 250ms entre tests VCC)
//   À MESURER sur prototype avant documentation finale
//
// Comparaison consommation:
//   | Version | Sleep (µA) | Gain vs V1.6.3 |
//   |---------|------------|----------------|
//   | V1.6.3  | ~500       | -              |
//   | V1.7.4  | ~500       | 0%             |
//   | V1.7.5+ | ~240       | -52%           |
//
// Compatibilité:
//   Drop-in replacement de V1.6.3 / V1.7.x
//   Hardware identique (Circuit V1.18)
//   Comportement identique (mêmes seuils ADC)
//
// Tests obligatoires avant release:
//   1. Cold-crank 6V → Doit entrer en safe mode puis reprendre
//   2. VCC variable 4.0-5.0V → Vérifier seuils 4.5V/4.7V
//   3. Conso mode OFF → Mesurer, doit être ~0,64mA total
//   4. Conso sleep → Mesurer ATtiny seul, doit être <0,3mA
//   5. Latence réponse → Vérifier ≤290ms acceptable
//   6. Taille flash → Compiler et vérifier marge disponible
//
// ═══════════════════════════════════════════════════════════════
// FUSES ATtiny85
// ═══════════════════════════════════════════════════════════════
// 
// Programmation:
//   ATtiny retiré, programmé sur adaptateur ISP externe
//   Pololu USB AVR Programmer v2.1 + Arduino IDE
//   Board: ATtiny85 @ 8MHz (internal)
//
// Valeurs fuses:
//   lfuse = 0xE2  → Oscillateur interne 8MHz
//   hfuse = 0xDF  → SPI enabled, EEPROM non préservée lors chip erase
//                   (EESAVE=1 = NON préservée, mais EEPROM non utilisée)
//   efuse = 0xFD  → BOD 2.7V (BODLEVEL[2:0]=101)
//
// Détail hfuse 0xDF = 1101 1111:
//   Bit 7: RSTDISBL = 1 → RESET activé (ISP possible)
//   Bit 6: DWEN     = 1 → debugWIRE désactivé
//   Bit 5: SPIEN    = 0 → SPI programming enabled
//   Bit 4: WDTON    = 1 → WDT pas forcé ON (contrôle software)
//   Bit 3: EESAVE   = 1 → EEPROM NON préservée lors chip erase
//   Bits 2:0: Réservés (=111)
//
// Détail efuse 0xFD = 1111 1101:
//   Bits 7:3: Réservés (=11111)
//   Bits 2:0: BODLEVEL = 101 → BOD 2.7V ✓
//
// Pourquoi BOD 2.7V (pas 4.3V):
//   - BOD 4.3V causerait un reset pendant cold-crank (VCC ~4.0-4.5V)
//   - BOD 2.7V laisse le code gérer le cold-crank via safe-mode
//   - Le gap 2.7V-4.5V est géré par:
//     * Au boot: readVCCmV() détecte et entre en safe-mode
//     * Pendant run: Ce gap n'est jamais atteint en usage normal
//       (batterie 12V → LDO 5V, ou batterie <6V → BOD reset)
//   - En cas de chute lente 5V→2.7V (LDO défaillant), le BOD reset
//     proprement avant comportement erratique
//
// ═══════════════════════════════════════════════════════════════
// CALIBRATION VBG (optionnelle pour haute précision)
// ═══════════════════════════════════════════════════════════════
//
// La référence bandgap interne varie de 1.0V à 1.2V selon:
//   - Exemplaire (variation fabrication)
//   - Température (-40°C à +85°C)
//
// Pour calibrer un exemplaire spécifique:
//   1. Mesurer VCC réel avec multimètre précis
//   2. Lire valeur ADC brute lors mesure bandgap
//   3. Calculer VBG_réel = VCC_mesuré × ADC / 1023
//   4. Modifier #define VBG_MV avec valeur calibrée
//   5. Optionnel: Stocker en EEPROM pour calibration par exemplaire
//
// Pour notre usage (seuils 4.5V/4.7V avec ±10% tolérance):
//   - Pire cas bas: VBG=1.0V → seuil 4.5V devient ~4.1V réel
//   - Pire cas haut: VBG=1.2V → seuil 4.5V devient ~4.9V réel
//   - Ces variations sont acceptables pour détection cold-crank
//   - Le circuit entre/sort de safe-mode un peu tôt/tard, pas critique
//
// ═══════════════════════════════════════════════════════════════
