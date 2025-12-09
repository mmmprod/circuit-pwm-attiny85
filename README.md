[![Version](https://img.shields.io/badge/version-1.7.10-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.18-green.svg)](hardware/)
[![Protocol](https://img.shields.io/badge/protocole-V9.28-purple.svg)](docs/Protocole_Test_PWM_V9_28.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/~10,5V avec hystérésis, optimisé pour environnement automotive.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## ⚠️ ALERTE — MIGRATION VERS V1.18

**Modifications V1.17 → V1.18 :**

| Composant | V1.17 | V1.18 (PRODUCTION) ⭐ |
|-----------|-------|------------------------|
| **D6** | Absent | **P6KE15CA** (TVS ESD) |
| **R13** | 10Ω 2W | **10Ω 5W** MOX |
| **C4** | 100µF 85°C | **100µF 105°C** 5000h |
| **C6** | 10µF 85°C | **10µF 105°C** 5000h |

**Rappel V1.15/V1.16 → V1.18:**
- R9 = **1kΩ** (pas 100Ω)
- R11 = **1kΩ**
- C12 = **10nF** sur J2

---

## 📋 Caractéristiques

### Hardware V1.18 — PRODUCTION ⭐
- **MCU**: ATtiny85-20PU (DIP-8, 8MHz interne)
- **Alimentation**: NCV2931 LDO 5V depuis 12V batterie (6-18V)
- **Input**: Filtrage RC double étage (-30dB @ 108Hz)
- **Output**: Driver BS170 + P-MOSFET FQP27P06 haut-côté
- **Protection entrée**: D1 anti-inversion, D2 TVS 1500W, F1 fusible, C12 EMI, **D6 TVS ESD**
- **Protection sortie**: D4 anti-backfeed, D5 TVS 600W, C11 EMI
- 🔴 **R9 = 1kΩ**, **R11 = 1kΩ**
- 🔴 **R13 = 10Ω 5W** (marge sécurité ×4)
- 🟢 **C4/C6 = 105°C 5000h** (durabilité automotive)

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
| [Circuit_PWM_uC_V1_18.md](hardware/schematic/Circuit_PWM_uC_V1_18.md) | **Schématique hardware V1.18** ⭐ |
| [Protocole_Test_PWM_V9_28.html](docs/Protocole_Test_PWM_V9_28.html) | Protocole de test V9.28 |
| [BOM_V1_18.csv](hardware/bom/BOM_V1_18.csv) | Liste composants V1.18 |

---

## 📊 Versions

### V1.18 (2025-12-09) - **HARDWARE PRODUCTION** ⭐
- 🔴 **D6** (P6KE15CA) — Protection ESD entrée PWM
- 🔴 **R13** : 2W → **5W** — Marge sécurité court-circuit ×4
- 🟢 **C4/C6** : 85°C → **105°C 5000h** — Durabilité automotive
- Audit ChatGPT WCCA/FMEA intégré
- 📁 [hardware/schematic/Circuit_PWM_uC_V1_18.md](hardware/schematic/Circuit_PWM_uC_V1_18.md)
- 📁 [hardware/bom/BOM_V1_18.csv](hardware/bom/BOM_V1_18.csv)

### V1.17 (2025-12-09) - ⚠️ REMPLACÉ PAR V1.18
- C12 (10nF) + R11 = 1kΩ

### V1.16 (2025-12-08)
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
**Version hardware**: V1.18 PRODUCTION  
**Version protocole**: V9.28  
**Dernière mise à jour**: 2025-12-09