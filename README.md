[![Version](https://img.shields.io/badge/version-1.7.8-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.15-green.svg)](hardware/)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/12V avec hystérésis, optimisé pour environnement automotive.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## 📋 Caractéristiques

### Hardware V1.15 — FINALE PRODUCTION ⭐
- **MCU**: ATtiny85-20PU (DIP-8, 8MHz interne)
- **Alimentation**: NCV2931 LDO 5V depuis 12V batterie (6-18V)
- **Input**: Filtrage RC double étage (-30dB @ 108Hz)
- **Output**: Driver BS170 + P-MOSFET FQP27P06 haut-côté
- **Protection entrée**: D1 anti-inversion, D2 TVS 1500W, F1 fusible
- **Protection sortie**: D4 anti-backfeed, D5 TVS 600W, C11 EMI ← NOUVEAU V1.15

### Firmware V1.7.8
- ✅ Timeout ADC (protection hardware bloqué)
- ✅ Protection overflow VCC (adc < 17)
- ✅ Détection ADC hors plage (fail-safe diviseur)
- ✅ Dummy read après réveil (stabilisation ADC)
- ✅ ADC désactivé pendant sleep (économie 260µA)
- ✅ volatile outputState (barrière mémoire)
- ✅ cli() atomique avant SREG restore
- ✅ Compatibilité PlatformIO (#include Arduino.h)
- ✅ Cold-crank auto-recovery (VCC monitoring)
- ✅ Latence activation: <1015ms (OFF→ON)
- ✅ Latence désactivation: <45ms (ON→OFF)
- ✅ Hystérésis symétrique: ±520mV

### Environnement automotive
- ✅ **Cold-crank**: 6V supporté
- ✅ **Load-dump**: 18V protégé
- ✅ **Température**: -15°C à +85°C
- ✅ **EMI**: Filtrage conforme

---

## 🚀 Quick Start

### 1. Matériel requis
- ATtiny85-20PU (DIP-8)
- Programmeur USBasp ou Pololu USB AVR Programmer
- Composants selon schéma V1.15

### 2. Configuration Arduino IDE
```
Board: ATtiny25/45/85 (ATTinyCore) 
Clock: 8 MHz Internal 
BOD: 2.7V
Programmer: USBasp
```

### 3. Programmation
```bash
# 1. Installer ATTinyCore dans Arduino IDE
# 2. Ouvrir firmware/PWM_Window_ATtiny85_V1_7_8/PWM_Window_ATtiny85_V1_7_8.ino
# 3. Tools → Burn Bootloader (fuses uniquement)
# 4. Sketch → Upload Using Programmer
```

📖 **Guides détaillés** :
- [PROGRAMMING.md](docs/PROGRAMMING.md) - Guide de programmation complet
- [UPDATE.md](docs/UPDATE.md) - Guide de mise à jour firmware/hardware

---

## 📊 Versions

### V1.15 (2025-12-07) - **HARDWARE FINALE** ⭐
- 🔴 **AJOUT:** D4 (1N5822) anti-backfeed sortie
- 🔴 **AJOUT:** D5 (P6KE18CA) TVS protection sortie
- 🔴 **AJOUT:** C11 (100nF) filtrage EMI sortie
- ✅ Protections sortie complètes (3 couches)
- ✅ Code compatible: V1.7.8
- 📁 Schématique: [hardware/schematic/Circuit_PWM_uC_V1_15.md](hardware/schematic/Circuit_PWM_uC_V1_15.md)

### V1.7.8 (2025-12-06) - **FIRMWARE ACTUEL** ⭐
- ✅ Timeout boucle ADC (protection hardware bloqué)
- ✅ Protection overflow VCC (évite calcul erroné si adc < 17)
- ✅ Détection ADC hors plage (fail-safe diviseur R1/R2)
- ✅ Dummy read après réveil (stabilisation ADC)
- ✅ Consommation repos: ~0,65mA
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_8/](firmware/PWM_Window_ATtiny85_V1_7_8/)

### V1.14 (2025-12-06)
- ✅ Code compatible: V1.7.8 (protections défensives)
- ⚠️ Hardware incomplet (manquait protections sortie)
- 📁 Schématique: [hardware/schematic/Circuit_PWM_uC_V1_14.md](hardware/schematic/Circuit_PWM_uC_V1_14.md)

### V1.10 (2025-12-05)
- 🔴 CORRIGÉ: Orientation Zener D3 (anode→GATE_P, cathode→+12V_PROT)
- ✅ Audit PREMORTEM V3.5 complet

### V1.6.3 (2025-11-14)
- ✅ Hystérésis symétrique ±520mV
- ✅ Fenêtre activation : 3,10V - 6,61V PWM

---

## 🛡️ Protections V1.15

### Entrée
| Composant | Rôle |
|-----------|------|
| F1 (0,5A) | Surintensité |
| D1 (1N5822) | Anti-inversion polarité |
| D2 (1.5KE18CA) | TVS surtension 1500W |

### Sortie (NOUVEAU V1.15)
| Composant | Rôle |
|-----------|------|
| D4 (1N5822) | Anti-backfeed depuis jauge |
| D5 (P6KE18CA) | TVS surtension 600W |
| C11 (100nF) | Filtrage EMI |

### Microcontrôleur
| Protection | Description |
|------------|-------------|
| R3 (470Ω) | Limite injection ADC < 1mA |
| BOD 2,7V | Reset propre sous-tension |
| Code V1.7.8 | Fail-safe diviseur défaillant |

---

## 📐 Architecture sortie V1.15

```
Q1 Drain → R13 (10Ω) → OUT_DRAIN → D4 (1N5822) → OUT_PROT → OUT_12V (J3)
                                                      │
                                                      ├── D5 (TVS) → GND
                                                      └── C11 (100nF) → GND
```

**Chute tension:** V_OUT = 12V - 0,07V - 1,0V - 0,3V = **10,6V @ 100mA** ✅

---

**Version firmware**: 1.7.8 (recommandé)  
**Version hardware**: V1.15 FINALE  
**Dernière mise à jour**: 2025-12-07