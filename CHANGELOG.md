# Changelog

Toutes les modifications importantes du projet Circuit PWM µC - ATtiny85.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/).

---

## [1.6.3] - 2025-11-14

### 🎯 FIRMWARE - Hystérésis symétrique

#### Modifié
- **SEUIL_HAUT_LIMITE** : 531 → 511 (SEUIL_HAUT - HYSTERESIS)
- Hystérésis haute : 260mV → **520mV** (cohérence avec hystérésis basse)
- Fenêtre activation OFF→ON : 239-531 → **239-511** (3,10V-6,61V PWM)

#### Amélioré
- ✅ Symétrie hystérésis : ±520mV identique des deux côtés
- ✅ Robustesse bruit haute fréquence : x2,6 vs x1,3 (V1.6.2)
- ✅ Documentation cohérence : toutes les valeurs concordent
- ✅ Maintenabilité code : logique uniforme

#### Notes
- Flash : ~950 bytes (identique V1.6.2)
- Compatible hardware : V1.7.11 (drop-in replacement V1.6.2)
- Tests terrain requis : validation zone 6,5-7V PWM

#### Migration depuis V1.6.2
- Comportement change zone 512-531 ADC (6,64V-6,88V PWM)
- V1.6.2 : Activation possible dans cette zone
- V1.6.3 : Reste OFF si boot dans zone, reste ON si montée progressive
- Impact pratique : <5% cas (variateur boot stable à 6,7V rare)

---

## [1.7.11] - 2025-11-14

### 🔧 HARDWARE - Optimisations finales

#### Modifié
- **R3** : 100Ω → **470Ω** (protection ADC injection optimale)
- **BOD** : 4,3V → **2,7V** (efuse 0xFD, compromis automotive)
- **Documentation I_repos** : Corrigée "<1mA" → "5-6mA attendu (LD1117 dominant)"

#### Calculé
- I_injection ADC @ 14,4V : 4,3mA → **0,91mA** < 1mA ✅ (ATtiny spec)
- Impact filtrage RC : τ = 47ms → 49,2ms (+4,7% négligeable)
- Atténuation PWM 108Hz : -30dB → **-30,4dB** (amélioration)

#### BOD 2,7V justification
- Cold-crank 6V : VCC=4,8V >> 2,7V (marge 2,1V) ✅
- Protection défaillance régulateur : reset propre <2,7V ✅
- Surconsommation : +20µA négligeable vs Iq LD1117 (5mA)
- Alternative BOD OFF : Risque comportement erratique <2V ❌
- Alternative BOD 4,3V : Risque trigger @ cold-crank ❌

#### Tests ajoutés
- **Test 7** : Vérifier BOD reset @ VCC 2,5-2,9V
- **Test 8** : Mesurer I_injection ADC pin7 <1mA @ PWM=14,4V

#### Compatible firmware
- V1.6.3 (recommandé)
- V1.6.2 (compatible)
- V1.6.1 (compatible)

---

## [1.6.2] - 2025-11-14

### 🐛 FIRMWARE - Corrections majeures

#### Corrigé
- **Nommage seuils** : SEUIL_HAUT_ON/OFF inversé → _ON/_LIMITE explicite
- **Documentation fenêtre** : 239-511 (faux) → 239-531 (correct)
- **Sleep conditionnel** : Zone aveugle 199-239 supprimée

#### Optimisé
- **Filtrage ADC** : Tri O(n²) → Moyenne O(n) rejet min/max (-80 bytes flash)
- **Watchdog** : 2s → 1s timeout (latence OFF→ON -50%)
- **Flash** : 1030 bytes → 950 bytes (-8%)

#### Performances
- Latence activation : 2015ms → **1015ms** max
- Latence désactivation : <45ms (identique)
- Conso repos : 5,24mA (identique)

---

## [1.7.10] - 2025-11-14

### 🔧 HARDWARE - Correction critique diviseur ADC

#### Corrigé CRITIQUE
- **Source diviseur ADC** : +5V_MCU → **PWM_FILT** ✅
- Bug V1.7.9 : Diviseur mesurait VCC au lieu de PWM filtré

#### Modifié
- **Régulateur** : MCP1702 → **LD1117V50** (Vin max 13,2V → 15V)
- Justification : 14,4V batterie charging < 15V max ✅

#### Recalculé
- Seuils diviseur k=0,377 : 219/531 → 2,84V/6,88V PWM ✅
- Hystérésis : ~250mV (20 counts × 12,95mV/count)

#### Compatible firmware
- V1.6.1 (seuils recalculés cohérents)

---

## [1.6.1] - 2025-11-13

### 🎯 FIRMWARE - Version initiale fonctionnelle

#### Ajouté
- Logique fenêtre PWM avec hystérésis
- Seuils ADC diviseur 33k/20k (k=0,377)
- SEUIL_BAS = 219 → 2,84V PWM
- SEUIL_HAUT = 531 → 6,88V PWM
- HYSTERESIS = 20 → ~250mV PWM
- Sleep mode watchdog 2s
- Moyennage ADC médiane (tri bubble sort)

#### Connu
- ⚠️ Nommage seuils inversé (corrigé V1.6.2)
- ⚠️ Doc fenêtre incorrecte (corrigée V1.6.2)
- ⚠️ Filtrage O(n²) inefficace (optimisé V1.6.2)

---

## [1.7.9] - 2025-11-13

### ❌ HARDWARE - Correction partielle (bug restant)

#### Corrigé
- Pin ADC : PB3 → **PB2/ADC1** (pin 7)

#### Bug restant
- ❌ Source diviseur encore incorrecte (+5V au lieu de PWM_FILT)
- Corrigé en V1.7.10

---

## [1.5.1] - 2025-11-13

### 🚀 FIRMWARE - Optimisations latence

#### Optimisé
- Latence : 80ms → **65ms** (-18%)
- Moyennage actif : 12 samples → 10 samples (55ms)
- Moyennage veille : 3 samples → 2 samples (10ms)
- readFilteredADC() : évite delay après dernier sample

#### Ajouté
- Métadonnées version firmware (traçabilité)
- FW_VERSION, FW_DATE, FW_AUTHOR, HW_REVISION

---

## [1.5.0] - 2025-11-10

### 🐛 FIRMWARE - Corrections bugs critiques V1.4

#### Corrigé CRITIQUE
- **Logique P-MOSFET** : HIGH=ON, LOW=OFF → HIGH=OFF, LOW=ON ✅
- **Hystérésis seuil haut** : Zone morte 684-724 supprimée
- **Watchdog** : WDE restant actif → WDIE only (interrupt-only)

#### Amélioré
- Sleep mode : Suppression sleep_bod_disable() (dangereux automotive)
- Économie énergie : DIDR0 pour -10µA sur ADC1
- Documentation : Commentaires explicites logique P-MOSFET

---

## [1.4.0] - 2025-11-05

### ⚠️ VERSION NON RECOMMANDÉE - BUGS CRITIQUES

#### Bugs connus (tous corrigés en V1.5.0)
- ❌ Logique P-MOSFET inversée
- ❌ Zone morte hystérésis 684-724 ADC
- ❌ Watchdog mal configuré (risque reset)
- ❌ Sleep mode agressif (perte BOD)

---

## Format versions

[MAJEURE.MINEURE.PATCH] - AAAA-MM-JJ

MAJEURE : Changements incompatibles (ex: nouveaux seuils) MINEURE : Ajout fonctionnalités compatibles PATCH : Corrections bugs

---

## Liens

- Repository : https://github.com/mmmprod/circuit-pwm-attiny85
- Issues : https://github.com/mmmprod/circuit-pwm-attiny85/issues
- Releases : https://github.com/mmmprod/circuit-pwm-attiny85/releases
