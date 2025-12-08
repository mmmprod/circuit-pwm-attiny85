[![Version](https://img.shields.io/badge/version-1.7.8-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.15-green.svg)](hardware/)
[![Protocol](https://img.shields.io/badge/protocole-V9.25-purple.svg)](docs/Protocole_Test_PWM_V9_25.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Conditionneur PWM automobile pour jauge Innovate Motorsports**

Convertit un signal PWM 12V 108Hz en sortie binaire 0V/~10,5V avec hystérésis, optimisé pour environnement automotive.

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
- **Protection sortie**: D4 anti-backfeed, D5 TVS 600W, C11 EMI ← V1.15

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
- ✅ **Load-dump**: 18V protégé (TVS clamp ~22V)
- ✅ **Température**: -15°C à +85°C
- ✅ **EMI**: Filtrage conforme

---

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Circuit_PWM_uC_V1_15.md](hardware/schematic/Circuit_PWM_uC_V1_15.md) | Schématique hardware V1.15 |
| [Protocole_Test_PWM_V9_25.html](docs/Protocole_Test_PWM_V9_25.html) | **Protocole de test complet V9.25** ⭐ |
| [PROGRAMMING.md](docs/PROGRAMMING.md) | Guide de programmation ATtiny85 |
| [UPDATE.md](docs/UPDATE.md) | Guide de mise à jour firmware/hardware |

### Protocole de Test V9.25 ⭐

Le protocole V9.25 inclut **14 phases de test** avec corrections audit PREMORTEM V3.6 :

| Phase | Description |
|-------|-------------|
| 0-1 | Vérifications visuelles + ohmmètre |
| 2-4 | Alimentation progressive, cold-crank, tensions |
| 5-6 | Courant repos, fail-safe diviseur, ADC clamp |
| 6.1 | **Fail-safe ADC limite haute (injection 13V)** 🆕 |
| 7 | Seuils fonctionnels (pot DC + PWM 108Hz JDS6600) |
| 8-9 | Charge 47Ω, court-circuit (critère <3s) |
| 10-11 | Backfeed D4, TVS D2+D5 avec vérification post-test |
| 12-13 | Latences oscillo, endurance 1h @ 14,4V |

**Nouveautés V9.25:**
- 🔴 Phase 6.1: Test fail-safe ADC > 950 (injection 13V sur PWM_IN)
- 🔴 Phase 9: Critère fusion fusible <3s (réaliste)
- 🔴 Phase 5.2: Recovery VCC <3s (cohérent WDT)
- 🟡 JDS6600 obligatoire (Phase 7.4)
- 🟡 Charge 120Ω / 3W minimum

---

## 🧪 Équipement de Test

### Obligatoire
- Alimentation labo 0-30V / 3A (limite courant)
- Multimètre précision ±0,5%
- Potentiomètre 10kΩ linéaire
- Charge 120Ω / **3W** + 47Ω / 5W
- **Générateur JDS6600** (Phase 7.4)

### Recommandé
- **RIDEN RD6006P** — Alimentation programmable 60V/6A
- **FNIRSI 1014D** — Oscilloscope 2 voies
- Thermomètre IR ±2°C

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

### Protocole V9.25 (2025-12-08) - **TESTS ACTUELS** ⭐
- 🔴 **Phase 6.1:** Test fail-safe ADC limite haute (injection 13V)
- 🔴 **Phase 9:** Critère fusion <3s (corrigé depuis <0,5s)
- 🔴 **Phase 5.2:** Recovery VCC <3s
- ✅ JDS6600 obligatoire pour Phase 7.4
- ✅ Charge 120Ω / 3W minimum
- ✅ Audité PREMORTEM V3.6
- 📁 [docs/Protocole_Test_PWM_V9_25.html](docs/Protocole_Test_PWM_V9_25.html)

---

## 🛡️ Protections V1.15

### Entrée
| Composant | Rôle |
|-----------|------|
| F1 (0,5A) | Surintensité |
| D1 (1N5822) | Anti-inversion polarité |
| D2 (1.5KE18CA) | TVS surtension 1500W |

### Sortie (V1.15)
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

**Chute tension:** V_OUT = 12V - 0,3V - 0,07V - 0,9V - 0,3V = **~10,5V @ 100mA** ✅

---

## 📁 Structure du repo

```
circuit-pwm-attiny85/
├── firmware/
│   └── PWM_Window_ATtiny85_V1_7_8/    # Code source V1.7.8
├── hardware/
│   ├── schematic/
│   │   ├── Circuit_PWM_uC_V1_15.md    # Schéma V1.15 FINALE
│   │   └── Circuit_PWM_uC_V1_14.md    # Schéma V1.14 (obsolète)
│   ├── bom/                            # Bill of Materials
│   └── pcb/                            # Design PCB (à venir)
├── docs/
│   ├── Protocole_Test_PWM_V9_25.html  # Protocole test V9.25 ⭐
│   ├── PROGRAMMING.md                  # Guide programmation
│   └── UPDATE.md                       # Guide mise à jour
└── README.md
```

---

**Version firmware**: 1.7.8  
**Version hardware**: V1.15 FINALE  
**Version protocole**: V9.25  
**Dernière mise à jour**: 2025-12-08