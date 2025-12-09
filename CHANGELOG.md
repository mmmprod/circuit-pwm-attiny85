# Changelog

Toutes les modifications importantes du projet Circuit PWM µC - ATtiny85.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/).

---
## [1.7.10] - 2025-12-09

### 🛡️ FIRMWARE - Cohérence timeout ADC

#### Ajouté
- **Timeout readADC()** : Cohérence avec readVCCmV() (protection hardware bloqué)
- **HW_REVISION mise à jour** : V1.16 (compatible avec correction critique R9=1kΩ)

#### Notes techniques
- Compatible hardware: **V1.16** (recommandé), V1.15, V1.14
- Drop-in replacement de V1.7.8
- Flash estimé: ~1250 bytes (identique V1.7.8)
- Consommation inchangée: ~0,65mA repos

#### Tests obligatoires
1. ADC bloqué (simulation) → timeout doit retourner 0
2. VCC hors plage (adc < 17) → doit retourner 0
3. Diviseur défaillant (ADC < 50 ou > 950) → sortie OFF
4. Cold-crank 6V → safe mode puis auto-recovery

---

## [1.16] - 2025-12-08

### 🔴 HARDWARE - CORRECTION CRITIQUE R9

#### Corrigé CRITIQUE
- **R9** : 100Ω → **1kΩ** (correction surchauffe)
- Bug V1.15 : R9=100Ω causait surchauffe (0,86W > 0,25W rating)
- Zener D3 conduit quand BS170 ON → 93mA avec 100Ω ❌
- Avec 1kΩ : 9,3mA → 0,086W < 0,25W rating ✅

#### Migration V1.15 → V1.16
**OBLIGATOIRE** : Remplacer R9 100Ω par 1kΩ immédiatement

**Procédure** :
1. Dessouder R9 (100Ω)
2. Souder nouvelle R9 (1kΩ, 1/4W, 1%)
3. Tester température R9 < 50°C après 5min fonctionnement
4. Valider avec protocole V9.26 (Phase 13 : test température)

#### Compatible firmware
- **V1.7.10** (recommandé)
- V1.7.8
- Toutes versions V1.7.x

#### Tests validation V9.26
- Phase 0 : Vérification R9=1kΩ (pas 100Ω)
- Phase 1 : Ohmmètre R9 ~1kΩ
- Phase 13 : Température R9 < 50°C après 5min

---

## [1.7.8] - 2025-12-06

### 🛡️ FIRMWARE - Protections défensives complètes

#### Ajouté
- **Timeout boucle ADC** : Protection contre ADC hardware bloqué (ADSC jamais clear)
- **Protection overflow VCC** : Si adc < 17 → retourne 0 (évite overflow uint32→uint16)
- **Détection ADC hors plage** : ADC < 50 ou > 950 → fail-safe sortie OFF
- **Dummy read après réveil** : Stabilisation ADC (datasheet p.146)

#### Protections V1.7.7 incluses
- Fail-safe diviseur R1/R2 défaillant (ADC hors plage normale)
- Scénarios détectés : R2 court-circuit GND, R1 court-circuit +5V, diviseur cassé

#### Protections V1.7.6 incluses
- Première conversion ADC ignorée après sleep (précision améliorée)

#### Notes techniques
- Compatible hardware: **V1.14** (recommandé), V1.11, V1.10
- Drop-in replacement de V1.7.5
- Flash estimé: ~1250 bytes (+50 bytes vs V1.7.5)
- Consommation inchangée: ~0,65mA repos

#### Tests obligatoires
1. ADC bloqué (simulation) → timeout doit retourner 0
2. VCC hors plage (adc < 17) → doit retourner 0
3. Diviseur défaillant (ADC < 50 ou > 950) → sortie OFF
4. Cold-crank 6V → safe mode puis auto-recovery

---

## [1.7.7] - 2025-12-06

### 🛡️ FIRMWARE - Détection défaut hardware

#### Ajouté
- **Détection ADC hors plage** : Fail-safe si diviseur R1/R2 défaillant
- Seuils de détection : ADC < 50 (~0,65V) ou > 950 (~12,3V)
- Comportement fail-safe : sortie OFF immédiate

#### Scénarios couverts
- R2 court-circuit vers GND → ADC ≈ 0 → détecté
- R1 court-circuit vers +5V → ADC ≈ 1023 → détecté
- Diviseur cassé/dessoudé → ADC erratique → détecté

#### Notes
- Auto-recovery : reteste à chaque réveil WDT (~1s)
- Compatible hardware: V1.14, V1.11, V1.10

---

## [1.7.6] - 2025-12-06

### 🔧 FIRMWARE - Stabilisation ADC

#### Ajouté
- **Dummy read après réveil** : Première conversion ignorée (stabilisation)
- Datasheet ATtiny85 p.146 : première conversion peut être imprécise après réactivation ADC

#### Impact
- Latence : +104µs par cycle (négligeable vs 1040ms total)
- Bénéfice : Lectures ADC plus stables, moins de jitter

---

## [1.7.5] - 2025-12-05

### 🔋 FIRMWARE - Optimisation consommation

#### Ajouté
- **ADC désactivé pendant sleep** : Économie ~260µA (datasheet ATtiny85 p.151)
- Sauvegarde/restauration ADCSRA dans enterSleepAtomic()

#### Amélioré
- Consommation sleep: ~500µA → ~240µA (-52%)
- Consommation totale repos: ~0,7mA → ~0,65mA

#### Notes
- Compatible hardware: **V1.11** (recommandé), V1.10
- Drop-in replacement de V1.7.4

---

## [1.11] - 2025-12-05

### 📄 HARDWARE - Documentation V1.11

#### Modifié
- Code compatible: V1.7.5 (vs V1.6.3 dans V1.10)
- Consommation documentée: ~0,65mA (vs ~0,7mA)

#### Notes
- Hardware physique identique à V1.10
- Mise à jour documentation uniquement

---
## [1.7.4] - 2025-12-05

### 🔒 FIRMWARE - Hardening final

#### Ajouté
- **volatile outputState** : Barrière mémoire explicite (programmation défensive)
- **cli() avant SREG restore** : Atomicité complète dans enterSleepAtomic()
- **#include <Arduino.h>** : Compatibilité PlatformIO/CLI explicite
- **#warning F_CPU** : Avertissement compilation si F_CPU != 8MHz

#### Amélioré
- **Documentation BOD 2,7V** : Explication détaillée choix fuses (BOD 2,7V vs 4,3V)
- **DIDR0 |=** : Préserve autres bits ADC (vs = qui écrase)
- **pinMode(PWM_IN, INPUT)** : Configuration explicite entrée ADC
- **Protection numSamples == 0** : Évite division par zéro dans readFilteredADC()
- **Macro MUX_BANDGAP** : Lisibilité sélection ADC bandgap
- **Prescaler F_CPU < 400kHz** : Couverture edge case ADC clock

#### Notes
- Flash : ~950 bytes (identique V1.7.3)
- Compatible hardware : **V1.10** (recommandé), V1.9, V1.7.11
- BOD 2,7V justifié : Cold-crank 6V → VCC=4,8V >> 2,7V (marge 2,1V) ✅
- Changements défensifs : Robustesse accrue, pas d'impact fonctionnel
- Version production finale : Hardening complet V1.7.x

#### Migration depuis V1.6.3/V1.7.3
- Drop-in replacement : Comportement identique
- Tests recommandés : Validation standard (VCC monitoring, BOD, sleep)
- Retour arrière : V1.7.3 ou V1.6.3 si problème compilation F_CPU

---
## [1.9.0] - 2025-11-24

### Hardware V1.9 - (NCV2931 LDO + final hardware tweaks)
- ✅ Régulateur NCV2931AD-5.0R2G adopté pour robustesse automotive (remplace LD1117 optionnel)
- ✅ R_LDO_IN = 10 Ω pour limiter pointes de courant (transitoires)
- ✅ Clarification des nœuds +12V_LDO_IN / +12V_PROT / FUSE_OUT
- ✅ Documentation complète: schéma, BOM (BOM_V1_9.csv) et protocole de test
- Notes: Compatible firmware V1.6.3. Voir hardware/schematic/Circuit_PWM_uC_V1_9.md et hardware/bom/BOM_V1_9.csv

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
