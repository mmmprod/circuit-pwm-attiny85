# 🔧 Guide de Programmation ATtiny85

Guide pas-à-pas pour programmer le firmware **V1.7.8** sur ATtiny85 avec Pololu USB AVR / USBasp.

**Circuit compatible :** V1.14  
**Dernière mise à jour :** 2025-12-06

---

## 📋 Table des matières

1. [Matériel requis](#-étape-0--matériel-requis)
2. [Retirer l'ATtiny du circuit](#-étape-1--retirer-lattiny85-du-circuit)
3. [Installer sur l'adaptateur](#-étape-2--installer-lattiny-sur-ladaptateur)
4. [Connecter le programmeur](#-étape-3--connecter-le-pololu-à-ladaptateur)
5. [Configurer Arduino IDE](#-étape-4--configurer-arduino-ide)
6. [Brancher USB](#-étape-5--brancher-le-programmeur-usb)
7. [Ouvrir le code](#-étape-6--ouvrir-le-code-v178)
8. [Vérifier la compilation](#-étape-7--vérifier-la-compilation)
9. [Graver les fuses](#-étape-8--graver-les-fuses-première-fois)
10. [Téléverser](#-étape-9--téléverser-le-code)
11. [Débrancher](#-étape-10--débrancher-le-programmeur)
12. [Remettre sur le circuit](#-étape-11--remettre-lattiny-sur-le-circuit)
13. [Test rapide](#-étape-12--test-rapide)
14. [Dépannage](#-dépannage)

---

## 📦 Étape 0 : Matériel requis

### Checklist matériel

- [ ] **Programmeur Pololu USB AVR** (ou USBasp / USBtinyISP)
- [ ] **Carte adaptateur ATtiny DIP-8** (avec support ZIF ou socket)
- [ ] **ATtiny85-20PU** retiré du circuit PWM
- [ ] **Câble USB** pour connecter le programmeur
- [ ] **Nappe 6 fils ISP** (connecteur 2×3 broches)
- [ ] **PC avec Arduino IDE 2.x**

---

## ⚡ Étape 1 : Retirer l'ATtiny85 du circuit

> ⚠️ **CIRCUIT HORS TENSION OBLIGATOIRE**  
> Débrancher l'alimentation AVANT de toucher à l'ATtiny !

### Checklist

- [ ] Alimentation débranchée de J1
- [ ] Attendre 10 secondes (décharge condensateurs)
- [ ] Repérer l'encoche de l'ATtiny (indique pin 1)
- [ ] Noter l'orientation actuelle !
- [ ] Extraire délicatement avec extracteur de CI ou tournevis plat

💡 **Astuce :** Faire levier alternativement de chaque côté pour ne pas tordre les pattes.

---

## 🔌 Étape 2 : Installer l'ATtiny sur l'adaptateur

> ⚠️ **ORIENTATION CRITIQUE** — Encoche vers le repère de l'adaptateur !

### Pinout ATtiny85 DIP-8

```
        ┌────────────┐
        │    ◠       │  ← Encoche (pin 1)
  Pin 1 │ RESET  VCC │ Pin 8
  Pin 2 │ PB3    PB2 │ Pin 7 (SCK/ADC1)
  Pin 3 │ PB4    PB1 │ Pin 6 (MISO)
  Pin 4 │ GND    PB0 │ Pin 5 (MOSI)
        └────────────┘
```

### Checklist

- [ ] Repérer le repère sur l'adaptateur (point, encoche ou "Pin 1")
- [ ] Aligner l'encoche de l'ATtiny avec le repère
- [ ] Insérer fermement mais délicatement
- [ ] Vérifier visuellement : aucune patte tordue

---

## 🔗 Étape 3 : Connecter le Pololu à l'adaptateur

### Connecteur ISP 6 broches (standard AVR)

```
Vue de face (côté broches) :

    ┌─────────────┐
    │  ●1    2●   │   1 = MISO    2 = VCC
    │  ●3    4●   │   3 = SCK     4 = MOSI
    │  ●5    6●   │   5 = RESET   6 = GND
    └─────────────┘
```

### Table de correspondance ISP → ATtiny85

| Pin ISP | Signal    | ATtiny85 Pin | Fil typique |
|---------|-----------|--------------|-------------|
| 1       | **MISO**  | Pin 6 (PB1)  | 🟡 Jaune    |
| 2       | **VCC**   | Pin 8        | 🔴 Rouge    |
| 3       | **SCK**   | Pin 7 (PB2)  | 🟢 Vert     |
| 4       | **MOSI**  | Pin 5 (PB0)  | 🔵 Bleu     |
| 5       | **RESET** | Pin 1 (PB5)  | 🟠 Orange   |
| 6       | **GND**   | Pin 4        | ⚫ Noir     |

### Checklist

- [ ] Connecter la nappe ISP à l'adaptateur (respecter orientation)
- [ ] Connecter l'autre extrémité au Pololu
- [ ] **NE PAS brancher l'USB encore**

---

## ⚙️ Étape 4 : Configurer Arduino IDE

### A) Installer le support ATtiny

1. Ouvrir Arduino IDE
2. **Fichier → Préférences**
3. Dans "URL de gestionnaire de cartes supplémentaires", ajouter :
   ```
   http://drazzy.com/package_drazzy.com_index.json
   ```
4. Cliquer **OK**
5. **Outils → Type de carte → Gestionnaire de cartes**
6. Chercher `ATTinyCore` par Spence Konde
7. Cliquer **Installer**

### B) Sélectionner la carte

- **Outils → Type de carte → ATTinyCore → ATtiny85 (No bootloader)**

### C) Configurer les options

| Option              | Valeur à sélectionner     |
|---------------------|---------------------------|
| Chip                | ATtiny85                  |
| Clock Source        | **8 MHz (internal)**      |
| Timer 1 Clock       | CPU (default)             |
| LTO                 | Enabled                   |
| millis()/micros()   | Enabled                   |
| BOD Level           | **BOD 2.7V** (recommandé) |
| Save EEPROM         | EEPROM retained           |

### D) Sélectionner le programmeur

- **Outils → Programmateur → USBasp**

💡 Si le Pololu n'est pas reconnu comme USBasp, essayer "USBtinyISP".

---

## 🔌 Étape 5 : Brancher le programmeur USB

### Checklist

- [ ] Brancher le câble USB au Pololu
- [ ] Brancher l'autre extrémité au PC
- [ ] Vérifier la LED du Pololu (verte ou bleue = OK)
- [ ] **Windows :** Attendre l'installation du driver

### Windows - Installation driver si problème

1. Télécharger [Zadig](https://zadig.akeo.ie/)
2. Lancer Zadig → **Options → List All Devices**
3. Sélectionner "USBasp" ou "Pololu"
4. Choisir driver `libusb-win32` ou `libusbK`
5. Cliquer **Install Driver**

---

## 📄 Étape 6 : Ouvrir le code V1.7.8

### Checklist

- [ ] **Fichier → Ouvrir**
- [ ] Naviguer vers `firmware/PWM_Window_ATtiny85_V1_7_8/PWM_Window_ATtiny85_V1_7_8.ino`
- [ ] Vérifier l'en-tête du fichier :

```cpp
/*
 * CIRCUIT PWM µC V1.7.8 - ATtiny85
 * ...
 */
#define FW_VERSION   "1.7.8"
#define FW_DATE      "2025-12-06"
```

- [ ] Confirmer que c'est bien la version **V1.7.8**

---

## ✔️ Étape 7 : Vérifier la compilation

### Checklist

- [ ] Cliquer sur **✓ (Vérifier)** ou `Ctrl+R`
- [ ] Attendre la compilation

### Résultat attendu ✅

```
Le croquis utilise 3XXX octets (XX%) de l'espace de stockage.
Les variables globales utilisent XXX octets (X%) de mémoire.
```

### Si erreur ❌

- Vérifier que ATTinyCore est installé
- Vérifier Type de carte = ATtiny85
- Vérifier Clock = 8 MHz internal

---

## 🔥 Étape 8 : Graver les fuses (première fois)

> ⚠️ **Cette étape est nécessaire SEULEMENT :**
> - Si c'est un ATtiny neuf (jamais programmé)
> - Ou si vous changez la configuration clock/BOD
>
> **Si vous reprogrammez un ATtiny déjà configuré, PASSER à l'étape 9.**

### Checklist

- [ ] **Outils → Graver la séquence d'initialisation** ("Burn Bootloader")
- [ ] Attendre le message de succès

### Résultat attendu ✅

```
avrdude: safemode: Fuses OK
```

### Fuses configurées

| Fuse   | Valeur | Fonction                          |
|--------|--------|-----------------------------------|
| lfuse  | 0xE2   | Clock 8MHz interne, startup 65ms  |
| hfuse  | 0xDF   | EEPROM retained, SPI enabled      |
| efuse  | 0xFD   | BOD 2.7V enabled                  |

---

## 🚀 Étape 9 : Téléverser le code

> ⚠️ **Utiliser "Téléverser avec un programmateur"**  
> PAS le bouton "Téléverser" normal !

### Checklist

- [ ] **Croquis → Téléverser avec un programmateur** (`Ctrl+Shift+U`)
- [ ] Attendre le téléversement (LED du programmeur clignote)

### Résultat attendu ✅

```
avrdude: AVR device initialized and ready to accept instructions
avrdude: writing flash (3XXX bytes):
avrdude: 3XXX bytes of flash written
avrdude: verifying flash memory against ...
avrdude: 3XXX bytes of flash verified
avrdude done.  Thank you.
```

---

## 🔌 Étape 10 : Débrancher le programmeur

### Checklist

- [ ] Débrancher le câble USB du PC
- [ ] Déconnecter la nappe ISP de l'adaptateur
- [ ] Retirer délicatement l'ATtiny de l'adaptateur

---

## 🔧 Étape 11 : Remettre l'ATtiny sur le circuit

> ⚠️ **ATTENTION À L'ORIENTATION !**  
> L'encoche de l'ATtiny doit être vers C6 (comme avant)

### Orientation sur le circuit PWM V1.14

```
    ┌──────────────────────────────────────────┐
    │                                          │
    │      C6 ○                                │
    │         ↑                                │
    │    ┌────┴────┐                           │
    │    │ ◠       │  ← Encoche vers C6        │
    │    │ ATtiny  │                           │
    │    │   85    │                           │
    │    └─────────┘                           │
    │                                          │
    │   Vers sortie OUT_12V →                  │
    │                                          │
    └──────────────────────────────────────────┘
```

### Checklist

- [ ] Vérifier que le circuit est TOUJOURS hors tension
- [ ] Aligner l'encoche de l'ATtiny vers C6
- [ ] Insérer délicatement dans le socket DIP-8
- [ ] Appuyer fermement pour bien enfoncer
- [ ] Vérification visuelle finale (aucune patte tordue)

---

## ✅ Étape 12 : Test rapide

### Checklist

- [ ] Rebrancher l'alimentation sur J1 (potentiomètre à 0V)
- [ ] Régler alimentation : **12V / limite 100mA**
- [ ] Allumer l'alimentation
- [ ] Mesurer **+5V_MCU = 4,9-5,1V** ✅
- [ ] Mesurer **I_repos < 1mA** (pot à 0V → sleep mode)
- [ ] Tourner le potentiomètre vers ~5V
- [ ] Vérifier **OUT_12V = ~12V** ✅

### 🎉 PROGRAMMATION RÉUSSIE !

L'ATtiny85 exécute maintenant le code **V1.7.8**.  
Procéder au protocole de test complet (V9.13).

---

## 🔧 Dépannage

| Problème | Solution |
|----------|----------|
| `avrdude: error: could not find USBasp` | Driver non installé → Utiliser Zadig |
| `target doesn't answer` | Vérifier orientation ATtiny, nappe ISP, port USB |
| `Signature mismatch` | Mauvaise carte → Vérifier Type de carte = ATtiny85 |
| Compilation échoue | ATTinyCore pas installé → Refaire étape 4 |
| +5V_MCU = 0V après remontage | ATtiny à l'envers → Retourner |
| OUT_12V toujours 0V | Code pas flashé → Reprogrammer |
| I_repos > 10mA | Ancien code → Reprogrammer V1.7.8 |

### Linux - Règle udev

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", MODE="0666"' | sudo tee /etc/udev/rules.d/99-usbasp.rules
sudo udevadm control --reload-rules
```

---

## 📚 Ressources

- [ATTinyCore GitHub](https://github.com/SpenceKonde/ATTinyCore)
- [ATtiny85 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)
- [Zadig Driver Tool](https://zadig.akeo.ie/)
- [Pololu USB AVR Programmer](https://www.pololu.com/product/3172)

---

## ✅ Checklist récapitulative

- [ ] ATTinyCore installé dans Arduino IDE
- [ ] Drivers installés (Windows)
- [ ] ATtiny85 retiré du circuit PWM
- [ ] ATtiny85 dans adaptateur (bonne orientation)
- [ ] Pololu connecté via nappe ISP
- [ ] USB branché
- [ ] Config Arduino IDE vérifiée (8MHz, BOD 2.7V)
- [ ] Burn Bootloader OK (1× seulement si ATtiny neuf)
- [ ] Compilation OK
- [ ] Upload OK (`avrdude done. Thank you.`)
- [ ] ATtiny retiré de l'adaptateur
- [ ] ATtiny réinstallé sur circuit (encoche vers C6)
- [ ] Test fonctionnel OK (+5V, I_repos, OUT_12V)

---

**Version guide :** 2.0  
**Code compatible :** V1.7.8  
**Hardware compatible :** V1.14  
**Dernière mise à jour :** 2025-12-06  
**Auteur :** mmmprod
