# Guide de mise à jour - Circuit PWM µC

Guide complet pour mettre à jour le firmware et le hardware du Circuit PWM ATtiny85.

---

## 📋 Avant de commencer

### Identifier votre version actuelle

Avant toute mise à jour, identifiez les versions installées :

**Version firmware :**
- Si vous avez accès au code source compilé, vérifiez `FW_VERSION` dans le fichier `.ino`
- Sinon, référez-vous à la date d'installation et consultez le [CHANGELOG](../CHANGELOG.md)

**Version hardware :**
- Vérifiez la version PCB marquée sur le circuit (ex: V1.7.11, V1.9)
- Consultez votre BOM ou schéma de référence

**Versions actuelles recommandées :**
- **Firmware** : V1.7.10 (2025-12-09) ⭐
- **Hardware** : V1.16 (2025-12-08) ⭐
- **Protocole** : V9.26 (2025-12-09) ⭐

> **Note** : V1.7.8 reste compatible. V1.7.10 apporte cohérence timeout readADC() et support HW_REVISION V1.16.

---

## 🔄 Mise à jour du firmware

### Option 1 : Mise à jour sur circuit existant (recommandé)

Si le circuit est déjà assemblé et fonctionnel :

#### Étape 1 : Retirer l'ATtiny85 du circuit

⚠️ **OBLIGATOIRE** : L'ATtiny85 doit être retiré du circuit pour être reprogrammé.

1. **Couper l'alimentation 12V**
2. Retirer l'ATtiny85 du socket (si utilisé)
3. Ou dessouder l'ATtiny85 du PCB (avec station de soudage + tresse à dessouder)

#### Étape 2 : Programmer la nouvelle version

1. Placer l'ATtiny85 dans l'adaptateur SOIC8→DIP8
2. Connecter l'USBasp selon le [guide de programmation](PROGRAMMING.md#connexions-hardware)
3. Ouvrir Arduino IDE avec la configuration correcte
4. Ouvrir le nouveau fichier firmware (ex: `firmware/PWM_V1_6_3/PWM_V1_6_3.ino`)
5. **NE PAS** refaire "Burn Bootloader" (les fuses sont déjà configurées)
6. **Sketch** → **Upload Using Programmer** (Ctrl+Shift+U)

#### Étape 3 : Réinstaller l'ATtiny85

1. Vérifier l'orientation (point/encoche pin 1)
2. Réinsérer dans le socket ou ressouder sur le PCB
3. Rebrancher l'alimentation 12V
4. Tester le fonctionnement

**Résultat attendu :**
```
avrdude: AVR device initialized and ready to accept instructions
avrdude: Device signature = 0x1e930b (ATtiny85)
avrdude: erasing chip
avrdude: writing flash (950 bytes):
avrdude: 950 bytes of flash written
avrdude: verifying flash memory against ...
avrdude: 950 bytes of flash verified
avrdude done. Thank you.
```

### Option 2 : Mise à jour avec ATtiny85 de rechange

Si vous avez un ATtiny85 de rechange non programmé :

1. Programmer le nouvel ATtiny85 selon le [guide de programmation complet](PROGRAMMING.md)
   - **Burn Bootloader** (1× seulement, pour configurer les fuses)
   - **Upload Using Programmer** (nouveau firmware)
2. Remplacer l'ATtiny85 dans le circuit
3. Conserver l'ancien ATtiny85 comme backup

---

## 🔧 Mise à jour du hardware

### Compatibilité firmware ↔ hardware

Vérifiez la compatibilité avant toute mise à jour :

| Firmware | Hardware compatible | Notes |
|----------|---------------------|-------|
| **V1.7.10** | V1.16, V1.15, V1.14 | ✅ Recommandé |
| V1.7.8 | V1.16, V1.14 | ✅ Compatible |
| V1.6.3 | V1.7.11, V1.9 | ⚠️ Anciennes versions hardware |
| V1.6.2 | V1.7.11, V1.9 | ⚠️ Anciennes versions |
| V1.6.1 | V1.7.10+ | ⚠️ Requiert diviseur ADC correct |
| V1.5.1 | V1.5+ | ⚠️ Anciennes versions |

### Mises à jour hardware mineures

Certaines améliorations hardware peuvent être faites sans refaire le PCB :

#### V1.15 → V1.16 (CRITIQUE - OBLIGATOIRE)

**⚠️ CORRECTION CRITIQUE R9** : Bug surchauffe V1.15

**Changements obligatoires :**
- R9 : 100Ω → **1kΩ** (CRITIQUE)
- Protocole test : V9.25 → V9.26 (ajout test température R9)

**Problème V1.15 :**
- R9=100Ω cause surchauffe (0,86W > 0,25W rating)
- Zener D3 conduit quand BS170 ON → 93mA
- Risque défaillance R9 par température excessive

**Solution V1.16 :**
- R9=1kΩ → 9,3mA → 0,086W < 0,25W ✅
- Température R9 < 50°C en fonctionnement normal

**Procédure migration :**
1. **Identifier version** : Mesurer R9 à l'ohmmètre (hors tension)
   - Si R9 ≈ 100Ω → V1.15 → **MIGRATION OBLIGATOIRE**
   - Si R9 ≈ 1kΩ → V1.16 → OK
2. **Dessouder R9** (100Ω)
3. **Souder nouvelle R9** (1kΩ, 1/4W, 1% tolérance)
4. **Test température** : Alimenter circuit 12V, attendre 5 minutes
   - Mesurer température R9 avec thermomètre IR ou tactile
   - Attendu : < 50°C
   - Si > 50°C : vérifier R9 = 1kΩ et soudure correcte
5. **Validation protocole V9.26** :
   - Phase 0 : Vérifier R9=1kΩ visuellement
   - Phase 1 : Ohmmètre R9 ~1kΩ
   - Phase 13 : Température R9 < 50°C après 5min

**Firmware compatible :**
- V1.7.10 (recommandé, HW_REVISION="V1.16")
- V1.7.8 et toutes versions V1.7.x

#### V1.7.11 → V1.9.0 (recommandé)

**Changements principaux :**
- Régulateur : LD1117V50 → NCV2931AD-5.0R2G (meilleure robustesse automotive)
- Résistance d'entrée LDO : 0Ω → 10Ω (R_LDO_IN)

**Procédure :**
1. Dessouder l'ancien régulateur LD1117V50
2. Souder le nouveau régulateur NCV2931AD-5.0R2G
3. Ajouter résistance 10Ω en entrée du régulateur (entre +12V_PROT et +12V_LDO_IN)
4. Vérifier alimentation +5V_MCU : 4.90-5.10V
5. Tester selon [protocole de test](PROGRAMMING.md#vérification-post-upload)

**Optionnel** : Si budget limité, LD1117V50 reste fonctionnel pour usage non critique.

#### V1.7.10 → V1.7.11

**Changements mineurs :**
- R3 : 100Ω → 470Ω (protection ADC optimale)
- BOD : 4.3V → 2.7V (fuses seulement, si refus reprogrammation)

**Procédure :**
1. Dessouder R3 (100Ω)
2. Souder nouvelle R3 (470Ω)
3. Optionnel : Reconfigurer fuses BOD 2.7V avec `Burn Bootloader`

### Mises à jour hardware majeures

Pour migrer depuis V1.5 ou antérieur vers V1.9 :

⚠️ **Refabrication PCB recommandée** : Trop de changements pour modification manuelle.

**Alternatives :**
1. Commander nouveau PCB V1.9 (fichiers Gerber dans `hardware/pcb/`)
2. Réutiliser composants compatibles de l'ancien PCB
3. Programmer nouvel ATtiny85 avec firmware V1.6.3

---

## 📊 Tableau des versions et chemins de mise à jour

### Firmware

```
V1.4.0 ❌ (ne pas utiliser)
  ↓
V1.5.0 → V1.5.1 (optimisations latence)
  ↓
V1.6.1 → V1.6.2 (corrections majeures)
  ↓
V1.6.3 (hystérésis symétrique)
  ↓
V1.7.8 (protections défensives)
  ↓
V1.7.10 ⭐ (timeout cohérent, HW V1.16)
```

### Hardware

```
V1.5 (diviseur ADC initial)
  ↓
V1.7.9 → V1.7.10 (correction diviseur ADC)
  ↓
V1.7.11 (R3=470Ω, BOD 2.7V)
  ↓
V1.9.0 (NCV2931, résistance LDO_IN)
  ↓
V1.14 (version intermédiaire)
  ↓
V1.15 ⚠️ (BUG R9=100Ω surchauffe)
  ↓
V1.16 ⭐ (CORRECTION R9=1kΩ)
```

---

## 🚀 Scénarios de mise à jour courants

### Scénario 1 : Firmware V1.7.8 → V1.7.10

**Raison :** Timeout readADC() cohérent, support HW_REVISION V1.16

**Procédure :**
1. Retirer ATtiny85 du circuit
2. Reprogrammer avec `firmware/PWM_Window_ATtiny85_V1_7_10/PWM_Window_ATtiny85_V1_7_10.ino`
3. **NE PAS** refaire "Burn Bootloader"
4. Upload Using Programmer
5. Réinstaller et tester

**Impact :** Aucun changement fonctionnel, amélioration défensive timeout.

### Scénario 2 : Hardware V1.15 → V1.16 (CRITIQUE)

**Raison :** Correction bug surchauffe R9

**Procédure :**
1. Identifier R9=100Ω avec ohmmètre
2. Dessouder R9 (100Ω)
3. Souder R9 (1kΩ, 1/4W, 1%)
4. Tester température < 50°C après 5min
5. Valider avec protocole V9.26

**Impact :** Sécurité critique, élimine risque défaillance R9.

### Scénario 3 : Firmware V1.6.2 → V1.6.3

**Raison :** Hystérésis symétrique améliorée (robustesse ×2.6)

**Procédure :**
1. Retirer ATtiny85 du circuit
2. Reprogrammer avec `firmware/PWM_V1_6_3/PWM_V1_6_3.ino`
3. **NE PAS** refaire "Burn Bootloader"
4. Upload Using Programmer
5. Réinstaller et tester

**Impact :** Comportement change légèrement dans zone 6.64V-6.88V PWM (voir CHANGELOG).

### Scénario 4 : Hardware V1.7.11 → V1.9.0

**Raison :** Régulateur automotive grade (NCV2931)

**Procédure :**
1. Commander régulateur NCV2931AD-5.0R2G
2. Commander résistance 10Ω 0805 SMD
3. Dessouder LD1117V50
4. Souder NCV2931AD-5.0R2G
5. Ajouter R 10Ω en série sur +12V_LDO_IN
6. Tester alimentation et protocole complet

**Impact :** Meilleure robustesse aux transitoires automotive.

### Scénario 5 : Système complet V1.5.x → V1.16 + V1.7.10

**Raison :** Migration vers version actuelle optimale

**Procédure :**
1. Vérifier schéma V1.5 vs V1.16 (diviseur ADC, filtrage, R9=1kΩ)
2. Si différences majeures : Commander nouveau PCB V1.16
3. Assembler nouveau PCB selon BOM V1.16 (**vérifier R9=1kΩ**)
4. Programmer ATtiny85 avec firmware V1.7.10 (Burn Bootloader + Upload)
5. Tests complets selon protocole V9.26 (inclut test température R9)

**Durée estimée :** 2-3h (assemblage) + 30min (programmation et tests)

---

## ✅ Vérification après mise à jour

### Checklist post-update

Après toute mise à jour firmware ou hardware :

- [ ] **Alimentation** : +5V_MCU = 4.90-5.10V
- [ ] **Boot** : OUT_CTRL = HIGH (~5V) au démarrage sans PWM
- [ ] **Seuils** : 
  - PWM < 2.84V → Sortie OFF
  - PWM 3-6V → Sortie ON
  - PWM > 6.88V → Sortie OFF
- [ ] **Hystérésis** : Pas d'oscillations entre ON/OFF
- [ ] **Consommation** : <0.8mA en mode sleep (PWM hors fenêtre)
- [ ] **Cold-crank** : Fonctionnel à 6V alim
- [ ] **Load-dump** : Survit à 18V transitoire

### Tests recommandés

Pour validation complète, voir [docs/PROGRAMMING.md - Vérification post-upload](PROGRAMMING.md#vérification-post-upload).

**Test rapide (5 minutes) :**
1. Alimenter 12V sans signal PWM → Sortie = 12V (P-MOSFET ON par défaut)
2. Appliquer PWM simulé 5V → Sortie = 12V (activation)
3. Appliquer PWM simulé 2V → Sortie = 0V (désactivation)
4. Retirer PWM → Sortie = 12V (retour repos)

---

## 🐛 Problèmes courants après mise à jour

### Comportement différent après update firmware

**Symptôme :** Sortie ON/OFF à des tensions différentes qu'avant.

**Cause probable :** Changement de seuils ou hystérésis entre versions.

**Solution :**
1. Consulter [CHANGELOG.md](../CHANGELOG.md) pour différences entre versions
2. V1.6.2 vs V1.6.3 : Zone 6.64V-6.88V change (hystérésis symétrique)
3. Si comportement problématique, revenir à version précédente

### ATtiny85 non détecté après réinstallation

**Symptôme :** Circuit ne démarre pas, +5V_MCU présent mais pas de sortie.

**Causes possibles :**
- Pin 1 mal orienté (ATtiny inversé)
- Pin plié lors de l'insertion
- Soudure froide ou contact défaillant

**Solution :**
1. Vérifier orientation (point/encoche pin 1 vers haut)
2. Inspecter pins au microscope/loupe
3. Retester ATtiny85 sur breadboard avec USBasp
4. Mesurer continuité pins ATtiny → pistes PCB

### Fuses incorrectes après "Burn Bootloader"

**Symptôme :** Circuit ne démarre pas ou clock incorrect.

**Cause :** Config Arduino IDE incorrecte lors du Burn Bootloader.

**Solution :**
1. Retirer ATtiny85 du circuit
2. Vérifier config Arduino IDE :
   - Clock : **8 MHz (internal)**
   - BOD : **Disabled** (ou 2.7V pour V1.7.11+)
3. Refaire "Burn Bootloader" avec config correcte
4. Re-upload firmware
5. Tester sur breadboard avant réinstallation

### Régulateur chauffe après remplacement

**Symptôme :** NCV2931 chauffe excessivement (>60°C).

**Causes possibles :**
- Court-circuit +5V_MCU → GND
- Condensateur C5 (100µF) défaillant
- Résistance R_LDO_IN manquante ou incorrecte

**Solution :**
1. Couper alimentation immédiatement
2. Mesurer résistance +5V_MCU ↔ GND (hors tension) : >1MΩ attendu
3. Vérifier condensateurs alim (C5, C6)
4. Vérifier R_LDO_IN = 10Ω présente
5. Si problème persiste, remplacer régulateur

---

## 📚 Ressources complémentaires

- **[PROGRAMMING.md](PROGRAMMING.md)** : Guide de programmation complet
- **[CHANGELOG.md](../CHANGELOG.md)** : Historique des versions et modifications
- **[README.md](../README.md)** : Vue d'ensemble du projet
- **Schémas** : `hardware/schematic/`
- **BOM** : `hardware/bom/`

---

## 🆘 Support

En cas de problème non résolu :

1. Vérifier [Issues GitHub](https://github.com/mmmprod/circuit-pwm-attiny85/issues) existantes
2. Ouvrir une nouvelle issue avec :
   - Version firmware actuelle et cible
   - Version hardware
   - Logs de programmation (avrdude)
   - Photos du circuit (si pertinent)
   - Mesures relevées (tensions, comportement)

---

**Version guide** : 2.0  
**Dernière mise à jour** : 2025-12-09  
**Auteur** : mmmprod
