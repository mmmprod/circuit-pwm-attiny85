[![Version](https://img.shields.io/badge/version-1.7.10-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.17-green.svg)](hardware/)
[![Protocol](https://img.shields.io/badge/protocole-V9.28-purple.svg)](docs/Protocole_Test_PWM_V9_28.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/~10,5V avec hystérésis, optimisé pour environnement automotive.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## ⚠️ ALERTE CRITIQUE — MIGRATION V1.15/V1.16 → V1.17

**Modifications requises selon votre version :**

| Composant | V1.15 (BUG) | V1.16 | V1.17 (PRODUCTION) ⭐ |
|-----------|-------------|-------|------------------------|
| **R9** | 100Ω ❌ | **1kΩ** ✅ | **1kΩ** ✅ |
| **R11** | 100Ω | 100Ω | **1kΩ** ✅ |
| **C12** | Absent | Absent | **10nF** ✅ |
| Risque R9 | SURCHAUFFE | Sécurisé | Sécurisé |

**Actions V1.16 → V1.17:**
1. Ajouter **C12** (10nF X7R 50V) sur J2 entre pin1 et pin2
2. Remplacer **R11** 100Ω par **1kΩ**

**Actions V1.15 → V1.17:**
1. Remplacer **R9** 100Ω par **1kΩ** (CRITIQUE)
2. Ajouter **C12** (10nF X7R 50V) sur J2
3. Remplacer **R11** 100Ω par **1kΩ**

---

## 📋 Caractéristiques

### Hardware V1.17 — PRODUCTION ⭐
- **MCU**: ATtiny85-20PU (DIP-8, 8MHz interne)
- **Alimentation**: NCV2931 LDO 5V depuis 12V batterie (6-18V)
- **Input**: Filtrage RC double étage (-30dB @ 108Hz)
- **Output**: Driver BS170 + P-MOSFET FQP27P06 haut-côté
- **Protection entrée**: D1 anti-inversion, D2 TVS 1500W, F1 fusible, **C12 EMI**
- **Protection sortie**: D4 anti-backfeed, D5 TVS 600W, C11 EMI
- 🔴 **R9 = 1kΩ** (correction critique V1.16)
- 🟡 **R11 = 1kΩ** (défense profondeur V1.17)
- 🟡 **C12 = 10nF** (EMI entrée V1.17)

### Firmware V1.7.10
- ✅ Timeout readADC() cohérent avec readVCCmV()
- ✅ Timeout ADC (protection hardware bloqué)
- ✅ Protection overflow VCC (adc < 17)
- ✅ Détection ADC hors plage (fail-safe diviseur)
- ✅ Cold-crank auto-recovery (VCC monitoring)
- ✅ Hystérésis symétrique: ±520mV

### Environnement automotive
- ✅ **Cold-crank**: 6V supporté
- ✅ **Load-dump**: 18V protégé (TVS clamp ~22V)
- ✅ **Température**: -15°C à +85°C

---

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Circuit_PWM_uC_V1_17.md](hardware/schematic/Circuit_PWM_uC_V1_17.md) | **Schématique hardware V1.17** ⭐ |
| [Protocole_Test_PWM_V9_28.html](docs/Protocole_Test_PWM_V9_28.html) | Protocole de test V9.28 |
| [BOM_V1_17.csv](hardware/bom/BOM_V1_17.csv) | Liste composants V1.17 |

---

## 📊 Versions

### V1.17 (2025-12-09) - **HARDWARE PRODUCTION** ⭐
- 🟡 **AJOUT C12** (10nF X7R 50V) — Protection EMI entrée J2
- 🟡 **R11** : 100Ω → **1kΩ** — Défense profondeur
- Audit WCCA/FMEA Gemini intégré
- 📁 [hardware/schematic/Circuit_PWM_uC_V1_17.md](hardware/schematic/Circuit_PWM_uC_V1_17.md)
- 📁 [hardware/bom/BOM_V1_17.csv](hardware/bom/BOM_V1_17.csv)

### V1.16 (2025-12-08) - ⚠️ REMPLACÉ PAR V1.17
- 🔴 **CORRECTION CRITIQUE:** R9 100Ω → **1kΩ**
- 📁 [hardware/schematic/Circuit_PWM_uC_V1_16.md](hardware/schematic/Circuit_PWM_uC_V1_16.md)

### V1.15 - ⚠️ OBSOLÈTE (bug R9)
- ⚠️ **BUG R9 = 100Ω → SURCHAUFFE**

### V1.7.10 - **FIRMWARE ACTUEL** ⭐
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_10/](firmware/PWM_Window_ATtiny85_V1_7_10/)

### V1.7.8 - Firmware précédent
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_8/](firmware/PWM_Window_ATtiny85_V1_7_8/)

---

**Version firmware**: 1.7.10  
**Version hardware**: V1.17 PRODUCTION  
**Version protocole**: V9.28  
**Dernière mise à jour**: 2025-12-09