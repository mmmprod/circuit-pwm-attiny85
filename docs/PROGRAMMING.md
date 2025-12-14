# 🔧 ATtiny85 Programming Guide

Step-by-step tutorial to flash firmware **V1.7.8** onto the ATtiny85 with a Pololu USB AVR / USBasp.

**Compatible board:** V1.14  
**Last update:** 2025-12-06

---

## 📋 Table of Contents

1. [Required hardware](#-step-0--required-hardware)
2. [Remove the ATtiny from the circuit](#-step-1--remove-the-attiny85-from-the-board)
3. [Seat it on the adapter](#-step-2--place-the-attiny-on-the-adapter)
4. [Wire the programmer](#-step-3--connect-the-pololu-to-the-adapter)
5. [Configure Arduino IDE](#-step-4--configure-arduino-ide)
6. [Plug in USB](#-step-5--plug-in-the-usb-programmer)
7. [Open the code](#-step-6--open-the-v178-code)
8. [Verify compilation](#-step-7--verify-compilation)
9. [Burn fuses](#-step-8--burn-fuses-first-time)
10. [Upload](#-step-9--upload-the-code)
11. [Disconnect](#-step-10--unplug-the-programmer)
12. [Reinstall on the PCB](#-step-11--reinstall-the-attiny-on-the-board)
13. [Quick test](#-step-12--quick-test)
14. [Troubleshooting](#-troubleshooting)

---

## 📦 Step 0: Required hardware

### Hardware checklist

- [ ] **Pololu USB AVR programmer** (or USBasp / USBtinyISP)
- [ ] **ATtiny DIP-8 adapter board** (ZIF or standard socket)
- [ ] **ATtiny85-20PU** removed from the PWM board
- [ ] **USB cable** for the programmer
- [ ] **6-wire ISP ribbon** (2×3 connector)
- [ ] **PC with Arduino IDE 2.x**

---

## ⚡ Step 1: Remove the ATtiny85 from the board

> ⚠️ **BOARD MUST BE POWERED OFF**  
> Unplug power BEFORE touching the ATtiny!

### Checklist

- [ ] Power removed from J1
- [ ] Wait 10 seconds (capacitors discharge)
- [ ] Locate the notch on the ATtiny (pin 1 indicator)
- [ ] Note the current orientation!
- [ ] Gently extract with an IC puller or flat screwdriver

💡 **Tip:** Pry up alternating sides to avoid bending the legs.

---

## 🔌 Step 2: Place the ATtiny on the adapter

> ⚠️ **ORIENTATION MATTERS** — notch toward the adapter marker!

### ATtiny85 DIP-8 pinout

```
        ┌────────────┐
        │    ◠       │  ← Notch (pin 1)
  Pin 1 │ RESET  VCC │ Pin 8
  Pin 2 │ PB3    PB2 │ Pin 7 (SCK/ADC1)
  Pin 3 │ PB4    PB1 │ Pin 6 (MISO)
  Pin 4 │ GND    PB0 │ Pin 5 (MOSI)
        └────────────┘
```

### Checklist

- [ ] Find the marker on the adapter (dot, notch, or “Pin 1”)
- [ ] Align the ATtiny notch with the marker
- [ ] Insert firmly but gently
- [ ] Visual check: no bent pins

---

## 🔗 Step 3: Connect the Pololu to the adapter

### 6-pin ISP header (standard AVR)

```
Front view (pin side):

    ┌─────────────┐
    │  ●1    2●   │   1 = MISO    2 = VCC
    │  ●3    4●   │   3 = SCK     4 = MOSI
    │  ●5    6●   │   5 = RESET   6 = GND
    └─────────────┘
```

### ISP → ATtiny85 mapping

| ISP Pin | Signal    | ATtiny85 Pin | Typical wire |
|---------|-----------|--------------|--------------|
| 1       | **MISO**  | Pin 6 (PB1)  | 🟡 Yellow    |
| 2       | **VCC**   | Pin 8        | 🔴 Red       |
| 3       | **SCK**   | Pin 7 (PB2)  | 🟢 Green     |
| 4       | **MOSI**  | Pin 5 (PB0)  | 🔵 Blue      |
| 5       | **RESET** | Pin 1 (PB5)  | 🟠 Orange    |
| 6       | **GND**   | Pin 4        | ⚫ Black     |

### Checklist

- [ ] Plug the ISP ribbon into the adapter (respect orientation)
- [ ] Connect the other end to the Pololu
- [ ] **Do not plug USB yet**

---

## ⚙️ Step 4: Configure Arduino IDE

### A) Install ATtiny support

1. Open Arduino IDE
2. **File → Preferences**
3. In “Additional Boards Manager URLs” add:
   ```
   http://drazzy.com/package_drazzy.com_index.json
   ```
4. Click **OK**
5. **Tools → Board → Boards Manager**
6. Search for `ATTinyCore` by Spence Konde
7. Click **Install**

### B) Select the board

- **Tools → Board → ATTinyCore → ATtiny85 (No bootloader)**

### C) Configure options

| Option              | Value to select          |
|---------------------|--------------------------|
| Chip                | ATtiny85                 |
| Clock Source        | **8 MHz (internal)**     |
| Timer 1 Clock       | CPU (default)            |
| LTO                 | Enabled                  |
| millis()/micros()   | Enabled                  |
| BOD Level           | **BOD 2.7V** (recommended) |
| Save EEPROM         | EEPROM retained          |

### D) Choose the programmer

- **Tools → Programmer → USBasp**

💡 If the Pololu is not recognized as USBasp, try “USBtinyISP”.

---

## 🔌 Step 5: Plug in the USB programmer

### Checklist

- [ ] Connect the USB cable to the Pololu
- [ ] Connect USB to the PC (LED on the programmer should light up)

---

## 💻 Step 6: Open the V1.7.8 code

1. **File → Open**
2. Navigate to `firmware/PWM_Window_ATtiny85_V1_7_8/`
3. Open the `.ino` file

---

## ✅ Step 7: Verify compilation

1. **Sketch → Verify/Compile** (or Ctrl+R)
2. Confirm “Done compiling” with no errors

---

## 🔥 Step 8: Burn fuses (first time only)

> Required only for a fresh ATtiny85. Skip if already done.

1. **Tools → Burn Bootloader** (configures fuses)
2. Wait for “Done burning bootloader”

---

## ⬆️ Step 9: Upload the code

1. **Sketch → Upload Using Programmer** (Ctrl+Shift+U)
2. Wait for `avrdude` to finish without errors

Expected output:
```
avrdude: AVR device initialized and ready to accept instructions
avrdude: Device signature = 0x1e930b (ATtiny85)
avrdude: erasing chip
avrdude: writing flash (...)
avrdude: verifying flash memory against ...
avrdude: ... bytes of flash verified
avrdude done. Thank you.
```

---

## 🔌 Step 10: Unplug the programmer

- Disconnect the USB cable
- Remove the ISP ribbon from the adapter

---

## 🔄 Step 11: Reinstall the ATtiny on the board

- Align the notch with the PCB marker
- Press the ATtiny85 back into the socket (or solder it)
- Visually check for correct orientation

---

## 🧪 Step 12: Quick test

1. Power the PWM board with 12V
2. Apply the PWM input signal
3. Verify the binary output toggles around ~10.5V with hysteresis

---

## 🛠️ Troubleshooting

| Issue | Possible causes | Fix |
|-------|-----------------|-----|
| `avrdude: initialization failed` | ISP cable miswired, wrong orientation | Recheck pin 1 alignment and ISP mapping |
| Compile error about missing core | ATTinyCore not installed | Reinstall via Boards Manager |
| Upload times out | Wrong programmer selected | Choose **USBasp** or **USBtinyISP** |
| Board not recognized over USB | Bad cable or port | Try another cable/port, avoid hubs |

---

### Glossary
- **Burn Bootloader**: in this workflow it only configures fuses (no bootloader is written).
- **Fuses**: low-level MCU configuration bits (clock source, brown-out detection, etc.).
- **ISP**: In-System Programming, the 6-wire AVR programming interface.
