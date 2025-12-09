[![Version](https://img.shields.io/badge/version-1.7.10-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.16-green.svg)](hardware/)
[![Protocol](https://img.shields.io/badge/protocole-V9.26-purple.svg)](docs/Protocole_Test_PWM_V9_26.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/~10,5V avec hystérésis, optimisé pour environnement automotive.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## ⚠️ ALERTE CRITIQUE — MIGRATION V1.15 → V1.16

**Si vous avez monté un circuit V1.15, vous DEVEZ remplacer R9:**

| Composant | V1.15 (BUG) | V1.16 (CORRIGÉ) |
|-----------|-------------|-----------------|
| **R9** | 100Ω | **1kΩ** |
| Dissipation | 0,86W > 0,25W ❌ | 0,086W < 0,25W ✅ |
| Risque | **SURCHAUFFE** | Sécurisé |

**Action:** Remplacer R9 100Ω par **1kΩ** immédiatement.

---

## 📋 Caractéristiques

### Hardware V1.16 — PRODUCTION ⭐
- **MCU**: ATtiny85-20PU (DIP-8, 8MHz interne)
- **Alimentation**: NCV2931 LDO 5V depuis 12V batterie (6-18V)
- **Input**: Filtrage RC double étage (-30dB @ 108Hz)
- **Output**: Driver BS170 + P-MOSFET FQP27P06 haut-côté
- **Protection entrée**: D1 anti-inversion, D2 TVS 1500W, F1 fusible
- **Protection sortie**: D4 anti-backfeed, D5 TVS 600W, C11 EMI
- 🔴 **R9 = 1kΩ** (correction critique V1.16)

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
| [Circuit_PWM_uC_V1_16.md](hardware/schematic/Circuit_PWM_uC_V1_16.md) | **Schématique hardware V1.16** ⭐ |
| [Protocole_Test_PWM_V9_26.html](docs/Protocole_Test_PWM_V9_26.html) | Protocole de test V9.26 |

---

## 📊 Versions

### V1.16 (2025-12-08) - **HARDWARE PRODUCTION** ⭐
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
**Version hardware**: V1.16 PRODUCTION  
**Version protocole**: V9.26  
**Dernière mise à jour**: 2025-12-09