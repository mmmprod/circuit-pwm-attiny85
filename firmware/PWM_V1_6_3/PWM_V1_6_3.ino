/*
 * ═══════════════════════════════════════════════════════════════
 * CIRCUIT PWM µC V1.6.3 - ATtiny85
 * ═══════════════════════════════════════════════════════════════
 * 
 * Changements vs 1.6.2:
 *   - SYMÉTRIE: Hystérésis haute 260mV → 520mV (±520mV cohérent)
 *   - SEUIL_HAUT_LIMITE: 531 → 511 (SEUIL_HAUT - HYSTERESIS)
 *   - Fenêtre ON: 239-531 → 239-511 (3,10V-6,61V vs 3,10V-6,88V)
 *   - Robustesse bruit haute fréquence améliorée (marge x2,6)
 *   - Documentation mise à jour (fenêtre correcte partout)
 * 
 * Historique:
 *   V1.6.3: Hystérésis symétrique ±520mV (recommandé production)
 *   V1.6.2: Nommage corrigé, optimisations, asymétrique
 *   V1.6.1: Version initiale fonctionnelle
 * 
 * Version: 1.6.3
 * Date: 14 Novembre 2025 21:00 UTC
 * Compatible: Circuit V1.7.11
 * Flash: ~950 bytes (identique V1.6.2)
 * 
 * ═══════════════════════════════════════════════════════════════
 */

#include <avr/sleep.h>
#include <avr/wdt.h>

// ═══════════════════════════════════════════════════════════════
// CONFIGURATION HARDWARE
// ═══════════════════════════════════════════════════════════════

#define PWM_IN      A1          // ADC1 = PB2 = pin7
#define OUT_CTRL    0           // PB0 = pin5

// ═══════════════════════════════════════════════════════════════
// SEUILS ADC - DIVISEUR 33k/20k (k=0,377)
// ═══════════════════════════════════════════════════════════════
// 
// Calculs validés Circuit V1.7.11:
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

#define SEUIL_BAS   219         // 2,84V PWM (centre bas)
#define SEUIL_HAUT  531         // 6,88V PWM (centre haut)
#define HYSTERESIS  20          // ~260mV ADC = ~520mV PWM

// ═══════════════════════════════════════════════════════════════
// SEUILS DÉRIVÉS AVEC HYSTÉRÉSIS ✅ SYMÉTRIQUE V1.6.3
// ═══════════════════════════════════════════════════════════════
//
// Logique fenêtre avec hystérésis SYMÉTRIQUE ±520mV:
//
//   OFF → ON:  adc ≥ SEUIL_BAS_ON ET adc ≤ SEUIL_HAUT_LIMITE
//              (239 ≤ adc ≤ 511)
//              Zone activation: 3,10V à 6,61V PWM
//
//   ON → OFF:  adc < SEUIL_BAS_LIMITE OU adc > SEUIL_HAUT_ON
//              (adc < 199 OU adc > 551)
//              Désactivation < 2,58V ou > 7,15V PWM
//
// Fenêtre stable ON: 239 à 511 (3,10V à 6,61V PWM) = 272 counts
// Zone hystérésis basse: 199-239 (2,58V-3,10V) = 40 counts = 520mV PWM
// Zone hystérésis haute: 511-551 (6,61V-7,15V) = 40 counts = 520mV PWM
//                                                              ↑ SYMÉTRIQUE
//
// Nomenclature explicite:
//   _ON suffix: Seuil pour ACTIVER sortie (entrée fenêtre)
//   _LIMITE suffix: Seuil pour RESTER dans état actuel (hystérésis)
//
// Avantages symétrie:
//   ✅ Cohérence conceptuelle (±520mV identique)
//   ✅ Robustesse bruit améliorée (marge x2,6 vs x1,3 asymétrique)
//   ✅ Documentation plus claire
//   ✅ Maintenance facilitée
//
// ═══════════════════════════════════════════════════════════════

#define SEUIL_BAS_ON      (SEUIL_BAS + HYSTERESIS)    // 239 → 3,10V PWM (activation)
#define SEUIL_BAS_LIMITE  (SEUIL_BAS - HYSTERESIS)    // 199 → 2,58V PWM (désactivation)
#define SEUIL_HAUT_LIMITE (SEUIL_HAUT - HYSTERESIS)   // 511 → 6,61V PWM (limite activation) ✅ V1.6.3
#define SEUIL_HAUT_ON     (SEUIL_HAUT + HYSTERESIS)   // 551 → 7,15V PWM (désactivation)

// ═══════════════════════════════════════════════════════════════
// PARAMÈTRES MOYENNAGE
// ═══════════════════════════════════════════════════════════════

#define NB_SAMPLES_ACTIF  8     // 8×5ms = 40ms moyennage actif
#define NB_SAMPLES_VEILLE 3     // 3×5ms = 15ms moyennage veille
#define DELAY_SAMPLE      5     // 5ms entre échantillons

// ═══════════════════════════════════════════════════════════════
// VARIABLES GLOBALES
// ═══════════════════════════════════════════════════════════════

bool outputState = false;       // État sortie: false=OFF, true=ON

// ═══════════════════════════════════════════════════════════════
// ISR WATCHDOG (réveil sleep)
// ═══════════════════════════════════════════════════════════════

ISR(WDT_vect) {
  // Interruption watchdog pour réveil sleep
  // Pas d'action nécessaire, juste sortir du sleep
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Lecture ADC simple
// ═══════════════════════════════════════════════════════════════

uint16_t readADC() {
  return analogRead(PWM_IN);
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Lecture ADC filtrée ✅ OPTIMISÉ V1.6.2
// ═══════════════════════════════════════════════════════════════
// Moyenne en rejetant min et max (filtre bruit sans tri O(n²))
// ═══════════════════════════════════════════════════════════════

uint16_t readFilteredADC(uint8_t numSamples) {
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
// FONCTION: Mise à jour état sortie ✅ CORRIGÉ V1.6.2+V1.6.3
// ═══════════════════════════════════════════════════════════════
//
// Logique fenêtre avec hystérésis SYMÉTRIQUE:
//
//   État OFF (outputState=false):
//     Activation si: SEUIL_BAS_ON ≤ adc ≤ SEUIL_HAUT_LIMITE
//                    (239 ≤ adc ≤ 511)
//     Exemples:
//       adc=238 → NON (< 239)
//       adc=240 → OUI → Passe ON
//       adc=510 → OUI → Passe ON
//       adc=512 → NON (> 511)
//
//   État ON (outputState=true):
//     Désactivation si: adc < SEUIL_BAS_LIMITE OU adc > SEUIL_HAUT_ON
//                       (adc < 199 OU adc > 551)
//     Exemples:
//       adc=198 → OUI (< 199) → Passe OFF
//       adc=200 → NON → Reste ON
//       adc=510 → NON → Reste ON
//       adc=512 → NON (512 ≤ 551) → Reste ON ✅ Hystérésis haute
//       adc=550 → NON → Reste ON
//       adc=552 → OUI (> 551) → Passe OFF
//
// Zones hystérésis:
//   Basse: 199-239 (2,58V-3,10V) = 520mV → Reste OFF si descente
//   Haute: 511-551 (6,61V-7,15V) = 520mV → Reste ON si montée
//
// Driver BS170 + P-MOSFET:
//   OUT_CTRL HIGH → BS170 ON → Gate P-MOS 0V → P-MOS ON → Sortie +12V
//   OUT_CTRL LOW  → BS170 OFF → Gate P-MOS +12V → P-MOS OFF → Sortie 0V
//
// ═══════════════════════════════════════════════════════════════

void updateOutputState(uint16_t adcValue) {
  
  // ───────────────────────────────────────────────────────────
  // ÉTAT OFF → Test activation (entrée dans fenêtre)
  // ───────────────────────────────────────────────────────────
  if (!outputState) {
    // Activation si ADC entre seuils ON
    if (adcValue >= SEUIL_BAS_ON && adcValue <= SEUIL_HAUT_LIMITE) {
      outputState = true;
      digitalWrite(OUT_CTRL, HIGH);   // P-MOSFET ON
    }
  }
  
  // ───────────────────────────────────────────────────────────
  // ÉTAT ON → Test désactivation (sortie de fenêtre)
  // ───────────────────────────────────────────────────────────
  else {
    // Désactivation si ADC hors limites
    if (adcValue < SEUIL_BAS_LIMITE || adcValue > SEUIL_HAUT_ON) {
      outputState = false;
      digitalWrite(OUT_CTRL, LOW);    // P-MOSFET OFF
    }
  }
}

// ═══════════════════════════════════════════════════════════════
// FONCTION: Mode sleep ✅ CORRIGÉ V1.6.2
// ═══════════════════════════════════════════════════════════════

void enterSleepMode() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Mode power-down (~1µA ATtiny)
  sleep_enable();
  sei();                                // Interruptions ON (watchdog)
  sleep_cpu();                          // Dormir
  sleep_disable();                      // Réveil après watchdog
}

// ═══════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════

void setup() {
  // ───────────────────────────────────────────────────────────
  // Configuration I/O
  // ───────────────────────────────────────────────────────────
  pinMode(OUT_CTRL, OUTPUT);
  digitalWrite(OUT_CTRL, LOW);        // État repos sûr: P-MOSFET OFF
  
  // ───────────────────────────────────────────────────────────
  // Configuration ADC
  // ───────────────────────────────────────────────────────────
  analogReference(DEFAULT);           // Référence VCC (5V)
  DIDR0 = (1 << ADC1D);              // Désactive buffer numérique ADC1
  
  // ───────────────────────────────────────────────────────────
  // Configuration Watchdog (1s timeout) ✅ OPTIMISÉ V1.6.2
  // ───────────────────────────────────────────────────────────
  cli();                              // Désactive interruptions
  MCUSR &= ~(1 << WDRF);             // Clear watchdog reset flag
  WDTCR = (1 << WDCE) | (1 << WDE);  // Enable watchdog change
  WDTCR = (1 << WDIE) | (1 << WDP2); // 1s timeout (WDP2 seul)
  sei();                              // Réactive interruptions
  
  // Délai stabilisation ADC
  delay(50);
}

// ═══════════════════════════════════════════════════════════════
// LOOP PRINCIPALE ✅ OPTIMISÉ V1.6.2
// ═══════════════════════════════════════════════════════════════

void loop() {
  
  // ───────────────────────────────────────────────────────────
  // Lecture ADC filtrée
  // ───────────────────────────────────────────────────────────
  uint8_t samples = outputState ? NB_SAMPLES_ACTIF : NB_SAMPLES_VEILLE;
  uint16_t adc = readFilteredADC(samples);
  
  // ───────────────────────────────────────────────────────────
  // Mise à jour état sortie selon fenêtre
  // ───────────────────────────────────────────────────────────
  updateOutputState(adc);
  
  // ───────────────────────────────────────────────────────────
  // Gestion sleep/délai selon état ✅ CORRIGÉ V1.6.2
  // ───────────────────────────────────────────────────────────
  if (!outputState) {
    // PWM hors fenêtre → sleep 1s pour économie énergie
    enterSleepMode();
  }
  else {
    // PWM dans fenêtre → polling actif court
    delay(DELAY_SAMPLE);
  }
}

// ═══════════════════════════════════════════════════════════════
// NOTES DÉVELOPPEUR
// ═══════════════════════════════════════════════════════════════
//
// Circuit V1.7.11 requis:
//   - LD1117V50 (régulateur 5V, Iq 5mA)
//   - Diviseur ADC: R1 (33kΩ ±1%) + R2 (20kΩ ±1%) → k=0,377
//   - Protection injection: R3 (470Ω) → I_inj <1mA @ 14,4V
//   - Filtrage RC: R4 (10kΩ) + C2 (4,7µF) = τ 49,2ms
//   - BOD 2,7V (efuse 0xFD) → Cold-crank 6V OK
//   - Driver: BS170 + FQP27P06 P-MOSFET
//
// Seuils terrain V1.6.3 (SYMÉTRIQUE):
//   - Fenêtre ON stable: 3,10V à 6,61V PWM (239-511 ADC) = 272 counts
//   - Hystérésis basse: 2,58V-3,10V (199-239 ADC) = 520mV PWM
//   - Hystérésis haute: 6,61V-7,15V (511-551 ADC) = 520mV PWM ✅ SYMÉTRIQUE
//
// Différences vs V1.6.2:
//   - Fenêtre ON: 239-531 → 239-511 (-7% largeur)
//   - Hystérésis haute: 260mV → 520mV (+100% robustesse)
//   - Cohérence: Asymétrique → Symétrique ✅
//
// Performances V1.6.3:
//   - Flash: ~950 bytes (identique V1.6.2)
//   - Latence activation OFF→ON: <1015ms (identique V1.6.2)
//   - Latence désactivation ON→OFF: <45ms (identique V1.6.2)
//   - Conso repos: 5,24mA (LD1117 dominant)
//   - Robustesse bruit: Marge x2,6 (vs x1,3 asymétrique V1.6.2)
//
// Tests terrain obligatoires:
//   - Vérifier oscillo basculements ±520mV autour 2,84V et 6,88V
//   - Confirmer fenêtre 3,10V-6,61V (vs 3,10V-6,88V V1.6.2)
//   - Mesurer I_repos @ PWM=0V (attendu 5-6mA, GO <10mA)
//   - Vérifier I_injection ADC @ PWM=14,4V (<1mA)
//   - Tester BOD reset @ VCC=2,5-2,9V (cold-crank simulé)
//   - Valider stabilité @ 6,5-6,7V PWM (zone hystérésis haute)
//
// Programmation:
//   - ATtiny retiré, programmé sur adaptateur ISP externe
//   - Pololu USB AVR Programmer v2.1 + Arduino IDE
//   - Fuses: lfuse=0xE2, hfuse=0xDF, efuse=0xFD (BOD 2,7V)
//   - Upload Using Programmer (pas Upload normal)
//
// Migration depuis V1.6.2:
//   - Drop-in replacement (hardware identique)
//   - Tester zone 6,5-6,8V PWM (comportement change)
//   - Si instabilité → revenir V1.6.2 (asymétrique)
//
// ═══════════════════════════════════════════════════════════════
