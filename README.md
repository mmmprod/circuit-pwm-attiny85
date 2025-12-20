[![Version](https://img.shields.io/badge/version-1.7.11-blue.svg)](https://github.com/mmmprod/circuit-pwm-attiny85/releases)
[![Hardware](https://img.shields.io/badge/hardware-V1.18-green.svg)](hardware)
[![Protocol](https://img.shields.io/badge/protocol-V9.3-purple.svg)](docs/Protocole_Test_PWM_V9_29.html)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Automotive PWM conditioner for Innovate Motorsports gauges**

Transforms a 12V 108Hz PWM signal into a binary 0V/~10.5V output with hysteresis—ruggedized for automotive abuse.

![ATtiny85](https://img.shields.io/badge/MCU-ATtiny85-red.svg)
![Automotive](https://img.shields.io/badge/automotive-grade-yellow.svg)

---

## ⚠️ ALERT — MIGRATE TO V1.18

**V1.17 → V1.18 updates:**

| Component | V1.17 | V1.18 (PRODUCTION) ⭐ |
|-----------|-------|----------------------|
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

### Firmware V1.7.11 — NEW ⭐

- ✅ **WDT 250ms** (was 1s) — Latency reduced ×4
- ✅ **Correction doc hfuse** — EESAVE=1 = NOT preserved
- ✅ readADC() timeout aligned with readVCCmV()
- ✅ ADC timeout (protection against stuck hardware)
- ✅ VCC overflow protection (adc < 17)
- ✅ Out-of-range ADC detection (fail-safe divider)
- ✅ Cold-crank auto-recovery (VCC monitoring)
- ✅ Symmetrical hysteresis: ±520mV

**Latency improvement V1.7.11:**
| Metric | V1.7.10 | V1.7.11 |
|--------|---------|---------|
| WDT period | 1000ms | 250ms |
| Worst-case latency | ~1040ms | ~290ms |
| Responsiveness | ×1 | **×3.6** |

### Automotive resilience

- ✅ **Cold-crank**: survives 6V
- ✅ **Load-dump**: protected to 18V (TVS clamp ~22V)
- ✅ **Temperature**: -15°C to +85°C

---

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Circuit_PWM_uC_V1_18.md](hardware/schematic/Circuit_PWM_uC_V1_18.md) | **Hardware schematic V1.18** ⭐ |
| [Protocole_Test_PWM_V9_3.html](docs/Protocole_Test_PWM_V9_3.html) | Test protocol V9.3 |
| [BOM_V1_18.csv](hardware/bom/BOM_V1_18.csv) | BOM V1.18 |



---

## 📊 Versions

### V1.7.11 (2025-12-19) - **CURRENT FIRMWARE** ⭐

- 🔴 **WDT 250ms** — Latency reduced from 1040ms to 290ms (×3.6 faster)
- 🔴 **Doc correction** — hfuse EESAVE=1 means EEPROM NOT preserved
- 📁 [firmware/PWM_Window_ATtiny85_V1_7_11/](firmware/PWM_Window_ATtiny85_V1_7_11)

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

### V1.7.10 (2025-12-09) - Previous firmware

- 📁 [firmware/PWM_Window_ATtiny85_V1_7_10/](firmware/PWM_Window_ATtiny85_V1_7_10)

### V1.7.8 - Legacy firmware

- 📁 [firmware/PWM_Window_ATtiny85_V1_7_8/](firmware/PWM_Window_ATtiny85_V1_7_8)

---

## 🔧 Quick Start

### Programming ATtiny85

1. **Remove** ATtiny85 from circuit board
2. **Place** in USBasp adapter or ATtiny dev board
3. **Program** with Arduino IDE:
   - Board: `ATtiny85 @ 8MHz (internal)`
   - Programmer: `USBasp` or `Arduino as ISP`
4. **Verify fuses**:
   ```
   lfuse = 0xE2  (8MHz internal)
   hfuse = 0xDF  (SPI enabled, EEPROM not preserved)
   efuse = 0xFD  (BOD 2.7V)
   ```
5. **Reinstall** ATtiny85 in DIP-8 socket

### Building the circuit

1. Use **V1.18** schematic and BOM
2. Critical components:
   - R9 = R11 = **1kΩ** (not 100Ω!)
   - R13 = **10Ω 5W MOX**
   - D6 = **P6KE15CA** (ESD protection)
3. Follow test protocol V9.3

---

## 📜 License

MIT License - See [LICENSE](LICENSE)

---

**Firmware version**: 1.7.11  
**Hardware version**: V1.18 PRODUCTION  
**Protocol version**: V9.3  
**Last update**: 2025-12-20
