# Circuit PWM µC - ATtiny85

[![Version](https://img.shields.io/badge/version-1.6.3-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.7.11-green.svg)](hardware/)
[![Hardware](https://img.shields.io/badge/hardware-V1.5-green.svg)](hardware/)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/12V avec hystérésis, optimisé pour environnement automotive.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## 📋 Caractéristiques

### Hardware
- **MCU**: ATtiny85-20SU (SOIC-8, 8MHz interne)
- **Alimentation**: Régulateur 5V depuis 12V batterie (6-18V)
- **Input**: Filtrage RC double étage (-76dB @ 108Hz)
- **Output**: Driver BS170 + P-MOSFET FQP27P06 haut-côté
- **Protection**: TVS, PTC auto-réarmant, ferrite EMI, clamps ESD

### Firmware V1.6.3
- ✅ **Latence activation**: <1015ms (OFF→ON, veille 1s)
- ✅ **Latence désactivation**: <45ms (ON→OFF, actif 8×5ms)
- ✅ **Hystérésis symétrique**: ±520mV (anti-rebond robuste)
- ✅ **Filtrage ADC optimisé**: Moyenne O(n) rejet min/max
- ✅ **Consommation repos**: <0.5mA (sleep mode)
- ✅ **Hystérésis**: ±200mV (anti-rebond)
- ✅ **Fenêtre activation**: 2.84V - 6.88V PWM
- ✅ **Ripple ADC**: <10mVpp garanti

### Environnement automotive
- ✅ **Cold-crank**: 6V supporté
- ✅ **Load-dump**: 18V protégé
- ✅ **Température**: -15°C à +85°C
- ✅ **EMI**: Filtrage conforme

---

## 🚀 Quick Start

### 1. Matériel requis
- ATtiny85-20SU (SOIC-8)
- Programmeur USBasp
- Adaptateur SOIC8→DIP8
- Composants selon [BOM](hardware/bom/BOM_V1_5.csv)

### 2. Configuration Arduino IDE
Board: ATtiny25/45/85 (ATTinyCore) 
Clock: 8 MHz Internal 
BOD: Disabled 
Programmer: USBasp


### 3. Programmation
```bash
# 1. Installer ATTinyCore dans Arduino IDE
# 2. Ouvrir firmware/PWM_V1_5_1/PWM_V1_5_1.ino
# 3. Tools → Burn Bootloader (1× seulement)
# 4. Sketch → Upload Using Programmer
