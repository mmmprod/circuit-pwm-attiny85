# Changelog

Toutes les modifications importantes du projet Circuit PWM µC - ATtiny85.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/).

---

## [1.5.1] - 2025-11-13

### Optimisé
- Latence réduite de 80ms à 65ms (amélioration de 18%)
- Moyennage actif: 12 samples → 10 samples (55ms)
- Moyennage veille: 3 samples → 2 samples (10ms)
- Fonction `readFilteredADC()` optimisée (pas de delay après dernier sample)

### Ajouté
- Métadonnées version firmware complètes
  - `FW_VERSION "1.5.1"`
  - `FW_DATE "2025-11-13"`
  - `FW_AUTHOR "mmmprod"`
  - `HW_REVISION "V1.5"`
- Documentation inline améliorée
- Footer avec spécifications performances

### Résumé performances V1.5.1
- Latence totale: 65ms (pire cas)
- Conso repos: <0.5mA
- Taille flash: ~1.2 KB (15% de 8KB)

---

## [1.5.0] - 2025-11-10

### Corrigé
- **CRITIQUE**: Logique P-MOSFET inversée (V1.4)
  - Ancien: `HIGH=ON`, `LOW=OFF` ❌
  - Nouveau: `HIGH=OFF`, `LOW=ON` ✅
- **CRITIQUE**: Hystérésis seuil haut avec zone morte 684-724 ADC
  - `SEUIL_HAUT_ON` passe de 724 à 684
  - `SEUIL_HAUT_OFF` passe de 684 à 724
  - Plus de zone morte dans la fenêtre
- Watchdog configuration risquée (WDE restant actif)
  - Configuration simplifiée avec WDIE only
  - Pas de risque de reset intempestif

### Amélioré
- Sleep mode simplifié
  - Suppression `sleep_bod_disable()` (dangereux automotive)
  - Suppression `delay(1)` inutile après réveil
- Économie énergie
  - Ajout `DIDR0 = (1 << ADC1D)` pour -10µA sur ADC1
- Documentation
  - Commentaires explicites logique P-MOSFET
  - Header complet avec pinout et config
  - Changelog inline détaillé

### Performances V1.5.0
- Latence: 80ms (pire cas)
- Conso repos: <0.5mA (vs ~5.2mA en V1.4)
- Tous bugs critiques V1.4 corrigés

---

## [1.4.0] - 2025-11-05

### ⚠️ VERSION NON RECOMMANDÉE - BUGS CRITIQUES

### Ajouté
- Version initiale production
- ATtiny85-20SU @ 8MHz
- Seuils: 2.84V - 6.88V avec hystérésis
- Sleep mode watchdog 500ms
- Moyennage ADC: 8 samples actif, 1 sample veille

### Bugs connus (corrigés en V1.5.0)
- ❌ Logique P-MOSFET inversée
  - `digitalWrite(OUT_CTRL, LOW)` en setup() → P-MOS ON au boot
  - `outputState=true` → `HIGH` → P-MOS OFF (inverse attendu)
- ❌ Zone morte hystérésis 684-724 ADC
  - Signal dans cette plage → sortie bloquée état précédent
- ❌ Watchdog mal configuré
  - `WDE=1` reste actif après config → risque reset
- ❌ Sleep mode agressif
  - `sleep_bod_disable()` → perte détection brownout automotive

### Performances V1.4.0
- Latence: 85ms
- Conso repos: ~5.2mA (pas de sleep effectif)
- ⚠️ Ne pas utiliser en production

---

## Format versions
[MAJEURE.MINEURE.PATCH] - AAAA-MM-JJ
MAJEURE: Changements incompatibles (ex: nouveaux seuils) 
MINEURE: Ajout fonctionnalités compatibles 
PATCH: Corrections bugs

---

## Liens

- Repository: https://github.com/mmmprod/circuit-pwm-attiny85
- Issues: https://github.com/mmmprod/circuit-pwm-attiny85/issues
- Releases: https://github.com/mmmprod/circuit-pwm-attiny85/releases
