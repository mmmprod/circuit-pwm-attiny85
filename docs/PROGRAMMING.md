# Guide de programmation ATtiny85

Guide complet pour programmer le firmware Circuit PWM µC sur ATtiny85-20SU.

---

## 📋 Matériel requis

### Hardware
- ✅ ATtiny85-20SU (SOIC-8)
- ✅ Programmeur USBasp
- ✅ Adaptateur SOIC8 vers DIP8 (ex: SA602, Aries 08-5120)
- ✅ Breadboard
- ✅ Câbles Dupont

### Software
- ✅ Arduino IDE 1.8.19 ou 2.x
- ✅ ATTinyCore (Spence Konde)
- ✅ Drivers USBasp (Windows uniquement)

---

## 🔧 Installation Arduino IDE

### 1. Installer ATTinyCore

**Via Board Manager (recommandé) :**
1. Arduino IDE → **File** → **Preferences**
2. **Additional Boards Manager URLs** :
http://drazzy.com/package_drazzy.com_index.json
3. **Tools** → **Board** → **Boards Manager**
4. Recherche : `ATTinyCore`
5. Installer **ATTinyCore by Spence Konde**

### 2. Installer drivers USBasp (Windows)

1. Télécharger [Zadig](https://zadig.akeo.ie/)
2. Brancher USBasp
3. Zadig → **Options** → **List All Devices**
4. Sélectionner **USBasp**
5. Driver : **libusbK (v3.x)**
6. **Replace Driver**

**Linux/Mac :** Aucun driver nécessaire

---

## 🔌 Connexions hardware

### Schéma de câblage
USBasp Adaptateur DIP8 (ATtiny85)

MOSI -------> Pin 5 (PB0/MOSI) 
MISO <------- Pin 6 (PB1/MISO) 
CK -------> Pin 7 (PB2/SCK) 
RESET -------> Pin 1 (PB5/RESET) 
VCC -------> Pin 8 (Vcc) 
GND -------> Pin 4 (GND)

### ATtiny85 pinout (vue dessus DIP8)
 ATtiny85
┌────┐
PB5 │1 8│ Vcc PB3 │2 7│ PB2 (SCK/ADC1) PB4 │3 6│ PB1 (MISO) GND │4 5│ PB0 (MOSI) └────┘


### ⚠️ Points d'attention

- ✅ **Vérifier polarité Vcc/GND**
- ✅ **USBasp en mode 5V** (jumper JP2 si présent)
- ✅ **Connexions courtes** (<15cm recommandé)
- ✅ **ATtiny85 hors circuit** (adaptateur seul sur breadboard)

---

## ⚙️ Configuration Arduino IDE

### Paramètres obligatoires
Tools → Board : ATtiny25/45/85 (No bootloader) 
Tools → Chip : ATtiny85 
Tools → Clock : 8 MHz (internal) 
Tools → B.O.D. : Disabled 
Tools → Save EEPROM : EEPROM retained 
Tools → Timer 1 Clock : CPU 
Tools → LTO : Enabled 
Tools → millis()/micros(): Enabled 
Tools → Programmer : USBasp


### Vérification config

**Port** : Ne PAS sélectionner de port COM (USBasp n'en a pas besoin)

---

## 🔥 Programmation fuses (1× seulement)

### Étape 1 : Burn Bootloader

⚠️ **ATTENTION** : Cette opération configure les fuses. À faire **UNE SEULE FOIS**.

1. Arduino IDE configuré comme ci-dessus
2. ATtiny85 dans adaptateur sur breadboard
3. USBasp connecté
4. **Tools** → **Burn Bootloader**

**Résultat attendu :**
avrdude: verifying ... 
avrdude: 1 bytes of lock verified 
avrdude: 1 bytes of efuse verified 
avrdude: 1 bytes of hfuse verified 
avrdude: 1 bytes of lfuse verified

### Fuses configurées

| Fuse | Valeur | Fonction |
|------|--------|----------|
| **lfuse** | 0xE2 | Clock 8MHz interne, startup 65ms |
| **hfuse** | 0xDF | EEPROM retained, Serial programming enabled |
| **efuse** | 0xFF | Self-programming disabled |

---

## 📤 Upload firmware

### Étape 1 : Ouvrir le sketch

1. **File** → **Open**
2. Naviguer vers : `firmware/PWM_V1_5_1/PWM_V1_5_1.ino`

### Étape 2 : Vérifier compilation

**Sketch** → **Verify/Compile**

**Résultat attendu :**
Sketch uses 1234 bytes (15%) of program storage space. 
Global variables use 26 bytes (5%) of dynamic memory.


### Étape 3 : Upload

**Sketch** → **Upload Using Programmer** (ou Ctrl+Shift+U)

**Résultat attendu :**
avrdude: AVR device initialized and ready to accept instructions 
avrdude: Device signature = 0x1e930b (ATtiny85) 
avrdude: erasing chip avrdude: reading input file ... 
avrdude: writing flash (1234 bytes): 
avrdude: 1234 bytes of flash written 
avrdude: verifying flash memory against ... 
avrdude: 1234 bytes of flash verified
avrdude done. 
Thank you.


---

## ✅ Vérification post-upload

### Test 1 : Retirer l'ATtiny

1. **Débrancher USBasp**
2. Retirer ATtiny85 de l'adaptateur
3. Vérifier visuellement les pins (pas de pliure)

### Test 2 : Test fonctionnel

Voir [docs/TESTING.md](TESTING.md) pour protocole complet.

**Test rapide :**
1. Souder ATtiny sur PCB (ou breadboard test)
2. Alimenter 12V
3. Mesurer pin 5 (OUT_CTRL) : doit être **HIGH (~5V)** au boot

---

## 🐛 Troubleshooting

### Erreur : "avrdude: error: could not find USB device"

**Cause** : USBasp non détecté

**Solutions** :
- Windows : Réinstaller drivers Zadig
- Linux : Ajouter règle udev :
  ```bash
  echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", MODE="0666"' | sudo tee /etc/udev/rules.d/99-usbasp.rules
  sudo udevadm control --reload-rules
  
  Vérifier câble USB (essayer autre port)
  Erreur : "avrdude: Device signature = 0x000000"
Cause : ATtiny85 non détecté

Solutions :

✅ Vérifier connexions MOSI/MISO/SCK/RESET/VCC/GND
✅ Vérifier jumper 5V sur USBasp
✅ Tester ATtiny85 avec multimètre (Vcc=5V, GND=0V)
✅ Essayer autre ATtiny85 (composant défectueux)
Erreur : "avrdude: verification error, first mismatch at byte 0x0000"
Cause : Écriture flash échoue

Solutions :

✅ Câbles trop longs (réduire à <10cm)
✅ Condensateur 100nF entre Vcc et GND de l'ATtiny
✅ Alimentation USBasp instable (utiliser alim externe 5V)
Warning : "can't set buffers for ..."
Pas grave : Message info libusb, pas d'impact sur programmation

Sketch trop gros : "Sketch too big"
Cause : Code > 8192 bytes

Solutions :

✅ Vérifier Tools → LTO : Enabled
✅ Supprimer code debug inutile
✅ Optimiser #define au lieu de variables
📝 Reprogrammation
Pour mettre à jour le firmware :

Retirer ATtiny du circuit (obligatoire)
Replacer dans adaptateur + USBasp
NE PAS refaire "Burn Bootloader" (fuses déjà OK)
Upload nouveau sketch directement
🔒 Protection lecture (optionnel)
Pour protéger le code contre la lecture :

⚠️ IRRÉVERSIBLE : L'ATtiny ne pourra plus être reprogrammé !
# Via avrdude command-line
avrdude -c usbasp -p attiny85 -U lock:w:0x0C:m
Non recommandé pour prototypage.

📚 Ressources
ATTinyCore GitHub
ATtiny85 Datasheet
USBasp Firmware
Zadig Driver Tool
✅ Checklist programmation
☐ ATTinyCore installé dans Arduino IDE
☐ Drivers USBasp installés (Windows)
☐ ATtiny85 dans adaptateur sur breadboard
☐ USBasp connecté (6 fils)
☐ Config Arduino IDE vérifiée (8MHz, BOD disabled)
☐ Burn Bootloader OK (1× seulement)
☐ Compile OK (~1.2KB)
☐ Upload OK (verification successful)
☐ ATtiny retiré adaptateur
☐ Test fonctionnel OK (pin 5 = HIGH au boot)
Version guide : 1.0
Dernière mise à jour : 2025-11-13
Auteur : mmmprod

**✅ Commit : "Add programming guide"**

---

## 📝 **FICHIER 5/10 : docs/TESTING.md**

**Add file** → **Create new file** → Nom: `docs/TESTING.md`

```markdown
# Protocole de tests - Circuit PWM µC

Tests obligatoires avant production et installation définitive.

---

## 🎯 Objectif

Valider le bon fonctionnement du circuit dans toutes les conditions automotive :
- ✅ Filtrage PWM
- ✅ Seuils et hystérésis
- ✅ Consommation repos
- ✅ Robustesse alimentations extrêmes
- ✅ Protection surtensions

---

## 🔧 Matériel de test

### Obligatoire
- ✅ Alimentation variable 0-20V / 1A
- ✅ Multimètre numérique
- ✅ Générateur PWM ou potentiomètre 10kΩ
- ✅ Circuit assemblé sur breadboard ou PCB

### Recommandé
- ✅ Oscilloscope (vérification ripple)
- ✅ Charge 12V 100mA (simuler jauge)
- ✅ Thermomètre IR (test chaleur)

---

## ⚡ TEST 1 : Vérification alimentation

### Objectif
Vérifier régulateur 5V et consommation à vide.

### Procédure

1. **Circuit hors tension**, multimètre en mode ohmmètre
2. Mesurer **+12V_PROT ↔ GND**
   - Attendu : **>100kΩ** (pas de court-circuit)
3. Mesurer **+5V_MCU ↔ GND**
   - Attendu : **>1MΩ** (régulateur OFF)

4. **Alimenter 12V** sans signal PWM
5. Mesurer **+12V_PROT**
   - Attendu : **11.7-12.3V**
6. Mesurer **+5V_MCU**
   - Attendu : **4.90-5.10V** ✅
7. Mesurer courant total
   - Attendu : **<0.8mA** (sleep mode actif)

### ✅ Critères validation
- [x] Pas de court-circuit
- [x] +5V_MCU stable ±100mV
- [x] Conso repos <0.8mA

---

## 📊 TEST 2 : Seuils et hystérésis

### Objectif
Valider fenêtre activation 2.84V-6.88V et hystérésis 200mV.

### Matériel
- Potentiomètre 10kΩ entre +12V et GND
- Curseur → DIM_IN

### Procédure

| PWM simulé | ADC théorique | Sortie attendue | Validation |
|------------|---------------|-----------------|------------|
| 0V | 0 | 0V (OFF) | ☐ |
| 2.5V | 256 | 0V (OFF, <2.84V) | ☐ |
| 2.9V | 297 | 12V (ON, >2.84V) | ☐ |
| 4.0V | 410 | 12V (ON, fenêtre) | ☐ |
| 6.5V | 665 | 12V (ON, fenêtre) | ☐ |
| 6.7V | 686 | 12V (ON, hystérésis) | ☐ |
| 7.1V | 726 | 0V (OFF, >6.88V) | ☐ |
| 6.9V | 706 | 0V (OFF, hystérésis) | ☐ |
| 10V | 1023 | 0V (OFF, hors fenêtre) | ☐ |

### Test hystérésis montée

1. PWM = 2.5V → Sortie = **0V**
2. Monter lentement PWM jusqu'à passage **0V → 12V**
3. Noter tension : **2.8-3.0V attendu** (seuil 2.84V + hystérésis)

### Test hystérésis descente

1. PWM = 4.0V → Sortie = **12V**
2. Descendre lentement PWM jusqu'à passage **12V → 0V**
3. Noter tension : **2.6-2.8V attendu** (seuil 2.84V - hystérésis)

### ✅ Critères validation
- [x] Sortie ON entre 2.84V et 6.88V
- [x] Hystérésis montée : ~2.9V
- [x] Hystérésis descente : ~2.7V
- [x] Écart montée/descente : 200mV ±50mV

---

## 🔄 TEST 3 : Latence et transitions

### Objectif
Mesurer temps de réponse aux changements PWM.

### Procédure

1. **Boot avec PWM=5V**
   - Alimenter circuit avec PWM déjà à 5V
   - Observer sortie avec chronomètre
   - Attendu : Sortie passe à **12V en <100ms**

2. **Transition rapide ON→OFF**
   - PWM stable 5V (sortie 12V)
   - Couper PWM à 0V brutalement
   - Mesurer temps jusqu'à sortie 0V
   - Attendu : **<100ms**

3. **Transition rapide OFF→ON**
   - PWM stable 0V (sortie 0V)
   - Monter PWM à 5V brutalement
   - Mesurer temps jusqu'à sortie 12V
   - Attendu : **<100ms**

### ✅ Critères validation
- [x] Boot <100ms
- [x] Transition ON→OFF <100ms
- [x] Transition OFF→ON <100ms

---

## 🌡️ TEST 4 : Cold-crank (6V)

### Objectif
Simuler démarrage moteur (chute tension batterie).

### Procédure

1. PWM = 5V, Sortie = 12V stable
2. **Réduire alimentation à 6V** progressivement
3. Mesurer **+5V_MCU**
   - Attendu : **4.90-5.10V** (régulateur stable)
4. Vérifier sortie reste **12V** (ou proportionnel ~6V)
5. Remonter alimentation à 12V
6. Vérifier sortie revient **12V**

### ✅ Critères validation
- [x] +5V_MCU stable à 6V alim
- [x] Circuit fonctionnel à 6V
- [x] Pas de reset ATtiny85

---

## ⚡ TEST 5 : Load-dump (18V)

### Objectif
Simuler surtension alternateur (déconnexion batterie).

### ⚠️ ATTENTION
Test destructif si protections défaillantes. Préparer fusible 0.5A.

### Procédure

1. PWM = 5V, Sortie = 12V stable
2. **Monter alimentation à 16V** progressivement
3. Mesurer **+5V_MCU**
   - Attendu : **4.90-5.10V**
4. **Pulse 18V pendant 100ms** (simuler load-dump)
5. Vérifier :
   - TVS 1.5KE18CA doit clamper <20V
   - +5V_MCU reste stable
   - Circuit ne reset pas
6. Redescendre à 12V
7. Vérifier sortie stable 12V

### ✅ Critères validation
- [x] +5V_MCU stable jusqu'à 18V
- [x] Pas de reset pendant pulse
- [x] TVS clamp <20V (oscillo si dispo)
- [x] Circuit fonctionnel après test

---

## 💤 TEST 6 : Consommation sleep mode

### Objectif
Valider conso <0.5mA hors fenêtre PWM.

### Procédure

1. **PWM = 0V** (hors fenêtre)
2. Multimètre en série sur alim 12V (mode µA)
3. Attendre 5 secondes (stabilisation sleep)
4. Mesurer courant total
   - Attendu : **<500µA**
5. Répéter avec **PWM = 10V** (hors fenêtre haute)
   - Attendu : **<500µA**
6. Comparer avec **PWM = 5V** (mode actif)
   - Attendu : **~5mA** (10× plus)

### ✅ Critères validation
- [x] Sleep <500µA (hors fenêtre)
- [x] Actif ~5mA (dans fenêtre)
- [x] Ratio actif/sleep >10×

---

## 📉 TEST 7 : Ripple filtrage (optionnel)

### Objectif
Vérifier filtrage RC -76dB @ 108Hz.

### Matériel
- Oscilloscope
- Générateur PWM 12V 108Hz 50%

### Procédure

1. Générateur PWM → DIM_IN
2. Oscillo CH1 : **DIM_IN** (avant filtrage)
   - Attendu : Carré 0-12V
3. Oscillo CH2 : **FILT2** (après double RC)
   - Attendu : Signal quasi-DC, ripple <20mVpp
4. Oscillo CH3 : **ADC_DIV** (entrée ATtiny)
   - Attendu : Ripple <10mVpp

### ✅ Critères validation
- [x] Ripple FILT2 <20mVpp
- [x] Ripple ADC_DIV <10mVpp
- [x] Atténuation conforme -76dB

---

## 🔥 TEST 8 : Température (optionnel)

### Objectif
Valider fonctionnement à 85°C (automotive).

### Procédure

1. Circuit en boîtier fermé
2. Alimenter 12V, PWM=5V, charge 100mA
3. Chauffer progressivement (sèche-cheveux ou étuve)
4. Mesurer température FQP27P06 et régulateur
5. Vérifier fonctionnement jusqu'à **85°C**
6. Surveiller +5V_MCU (pas de dérive)

### ✅ Critères validation
- [x] Fonctionnel jusqu'à 85°C
- [x] Pas de reset thermique
- [x] +5V_MCU stable ±5%

---

## 📋 Checklist complète
☐ TEST 1 : Alimentation (pas court-circuit, +5V OK) 
☐ TEST 2 : Seuils 2.84V-6.88V validés 
☐ TEST 3 : Latence <100ms 
☐ TEST 4 : Cold-crank 6V OK 
☐ TEST 5 : Load-dump 18V OK 
☐ TEST 6 : Sleep <0.5mA 
☐ TEST 7 : Ripple <10mVpp (optionnel) 
☐ TEST 8 : Température 85°C (optionnel)

---

## 🚨 Que faire si un test échoue ?

### +5V_MCU instable
- Vérifier condensateurs 100µF + 100nF alim
- Tester autre régulateur (LD1117 défectueux ?)

### Seuils incorrects
- Vérifier diviseur ADC (2× R 10kΩ)
- Vérifier clamps BAT85 (court-circuit ?)
- Reprogrammer ATtiny (mauvais firmware ?)

### Conso repos élevée
- Vérifier sleep mode code (watchdog actif ?)
- Vérifier régulateur Iq (LD1117 = 5mA, MIC5219 = 70µA)

### Reset intempestif
- Vérifier pull-up 10kΩ pin RESET
- Vérifier TVS (court-circuit ?)
- Tester autre ATtiny85

---

**Version protocole** : 1.0  
**Dernière mise à jour** : 2025-11-13  
**Auteur** : mmmprod
