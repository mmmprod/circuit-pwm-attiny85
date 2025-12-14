[![Version](https://img.shields.io/badge/version-1.7.10-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.18-green.svg)](hardware/)
[![Protocol](https://img.shields.io/badge/protocol-V9.28-purple.svg)](docs/Protocole_Test_PWM_V9_28.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Automotive PWM conditioner for Innovate Motorsports gauges**

Transforms a 12V 108Hz PWM signal into a binary 0V/~10.5V output with hysteresis—ruggedized for automotive abuse.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## ⚠️ ALERT — MIGRATE TO V1.18

**V1.17 → V1.18 updates:**

| Component | V1.17 | V1.18 (PRODUCTION) ⭐ |
|-----------|-------|------------------------|
| **D6** | Absent | **P6KE15CA** (ESD TVS) |
| **R13** | 10Ω 2W | **10Ω 5W** MOX |
| **C4** | 100µF 85°C | **100µF 105°C** 5000h |
| **C6** | 10µF 85°C | **10µF 105°C** 5000h |

**Reminder V1.15/V1.16 → V1.18:**
- R9 = **1kΩ** (not 100Ω)
- R11 = **1kΩ**
- C12 = **10nF** on J2

---

## 📋 Features

### Hardware V1.18 — PRODUCTION ⭐
- **MCU**: ATtiny85-20PU (DIP-8, internal 8MHz)
- **Power**: NCV2931 5V LDO from 12V battery (6-18V)
- **Input**: Dual-stage RC filter (-30dB @ 108Hz)
- **Output**: BS170 driver + FQP27P06 P-MOSFET high-side
- **Input protection**: D1 reverse, D2 TVS 1500W, F1 fuse, C12 EMI, **D6 ESD TVS**
- **Output protection**: D4 anti-backfeed, D5 TVS 600W, C11 EMI
- 🔴 **R9 = 1kΩ**, **R11 = 1kΩ**
- 🔴 **R13 = 10Ω 5W** (×4 safety margin)
- 🟢 **C4/C6 = 105°C 5000h** (automotive longevity)

### Firmware V1.7.10
- ✅ readADC() timeout aligned with readVCCmV()
- ✅ ADC timeout (protection against stuck hardware)
- ✅ VCC overflow protection (adc < 17)
- ✅ Out-of-range ADC detection (fail-safe divider)
- ✅ Cold-crank auto-recovery (VCC monitoring)
- ✅ Symmetrical hysteresis: ±520mV

### Automotive resilience
- ✅ **Cold-crank**: survives 6V
- ✅ **Load-dump**: protected to 18V (TVS clamp ~22V)
- ✅ **Temperature**: -15°C to +85°C

---

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Circuit_PWM_uC_V1_18.md](hardware/schematic/Circuit_PWM_uC_V1_18.md) | **Hardware schematic V1.18** ⭐ |
| [Protocole_Test_PWM_V9_28.html](docs/Protocole_Test_PWM_V9_28.html) | Test protocol V9.28 |
| [BOM_V1_18.csv](hardware/bom/BOM_V1_18.csv) | BOM V1.18 |

---

## 📊 Versions

### V1.18 (2025-12-09) - **HARDWARE PRODUCTION** ⭐
- 🔴 **D6** (P6KE15CA) — PWM input ESD protection
- 🔴 **R13** : 2W → **5W** — Short-circuit safety margin ×4
- 🟢 **C4/C6** : 85°C → **105°C 5000h** — Automotive durability
- ChatGPT audit WCCA/FMEA integrated
- 📁 [hardware/schematic/Circuit_PWM_uC_V1_18.md](hardware/schematic/Circuit_PWM_uC_V1_18.md)
- 📁 [hardware/bom/BOM_V1_18.csv](hardware/bom/BOM_V1_18.csv)

### V1.17 (2025-12-09) - ⚠️ REPLACED BY V1.18
- C12 (10nF) + R11 = 1kΩ

### V1.16 (2025-12-08)
- 🔴 **CRITICAL FIX:** R9 100Ω → **1kΩ**
- 📁 [hardware/schematic/Circuit_PWM_uC_V1_16.md](hardware/schematic/Circuit_PWM_uC_V1_16.md)

### V1.15 - ⚠️ OBSOLETE (R9 bug)
- ⚠️ **BUG R9 = 100Ω → OVERHEATING**

### V1.7.10 - **CURRENT FIRMWARE** ⭐
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_10/](firmware/PWM_Window_ATtiny85_V1_7_10/)

### V1.7.8 - Previous firmware
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_8/](firmware/PWM_Window_ATtiny85_V1_7_8/)

---

**Firmware version**: 1.7.10
**Hardware version**: V1.18 PRODUCTION
**Protocol version**: V9.28
**Last update**: 2025-12-09
