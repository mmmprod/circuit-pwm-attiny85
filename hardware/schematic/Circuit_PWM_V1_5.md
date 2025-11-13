# CIRCUIT PWM µC V1.5 FINAL

**Objectif:** Lisser PWM 12V 108Hz et piloter P-MOSFET via ATtiny85  
**Tenue:** Cold-crank 6V, load-dump 18V, Ta 85°C

**CHANGEMENTS V1.4 → V1.5:**
- ✅ Sortie: R 10Ω → **PTC 0,5A + Ferrite 0805 + 100nF + P6KE18CA**
- ✅ Entrée: **Supprimé ferrite série DIM** (EMI déjà filtré par RC)

_______________________________________________________________________________

## BLOC A – ALIM + PROTECTION
+12V_ACC → Fusible 0,5A → SB2100 (anode→cathode) → +12V_PROT 
+12V_PROT → TVS 1,5KE18CA ↔ GND 
+12V_PROT → C 100µF 50V + C 100nF → GND

_______________________________________________________________________________

## BLOC R – RÉGULATEUR 5V

**MIC5219-5.0YM5-TR (SOT-23-5) ou LD1117V50 (TO-220):**
- Pin IN: +12V_PROT → C 10µF + C 100nF → GND (≤10mm) 
- Pin OUT: +5V_MCU → C 10µF + C 100nF → GND (≤10mm) 
- Pin EN: +12V_PROT Pin GND: GND

**Spécifications:** 
- Vin 6-16V, Vout 5,0V ±2%, Iq 70µA, Iout 500mA

_______________________________________________________________________________

## BLOC µC – ATTINY85

**ATtiny85-20SU (SOIC-8):**
- Pin 8 (Vcc): +5V_MCU 
- Pin 7 (PB2/ADC1): Lecture PWM filtré 
- Pin 5 (PB0): Commande BS170 
- Pin 4 (GND): GND 
- Pin 1 (PB5/RESET): Pull-up 10kΩ interne

Découplage: 
- C 10µF + C 100nF entre pins 8-4 (≤10mm)

_______________________________________________________________________________

## BLOC F – FILTRAGE PWM

**⚠️ V1.5: SANS ferrite série entrée**
- FILT2 → R 10kΩ → ADC_DIV → R 10kΩ → GND ADC_DIV → Pin 7 ATtiny85

Clamps ESD:
- BAT85 anode→ADC_DIV, cathode→+5V_MCU
- BAT85 cathode→ADC_DIV, anode→GND

**Conversion:** 
ADC_10bit = (V_PWM / 2) × (1023 / 5,0V)

_______________________________________________________________________________

## BLOC OUTPUT – DRIVER + P-MOSFET

**BS170 (TO-92) driver N-MOS:**
- Pin 2 (Gate): ATtiny pin 5 via R 100Ω 
- Pin 2 (Gate): R 10kΩ → GND (pull-down) 
- Pin 1 (Source): GND 
- Pin 3 (Drain): GATE_DRIVE

**FQP27P06 (TO-220) commutation P-MOS:**
- Pin 2 (Source): +12V_PROT 
- Pin 1 (Gate): GATE_DRIVE 
- Pin 1 (Gate): R 10kΩ → +12V_PROT (pull-up) 
- Pin 3 (Drain): OUT_MOSFET

Protection gate: Zener 1N4733A 5,1V 1W:

- CATHODE → Pin 2 (Source +12V)
- ANODE → Pin 1 (Gate)

Diode roue libre: 
SB2100 anode→Drain, cathode→Source

_______________________________________________________________________________

## BLOC J – SORTIE (NOUVEAU V1.5)

**Architecture automotive complète:**
- OUT_MOSFET → PTC 0,5A 0805 → Ferrite 0805 → nœud OUT+

- OUT+ → C 100nF X7R → GND (filtrage HF) 
- OUT+ → TVS P6KE18CA ↔ GND (protection locale) 
- OUT+ → Bornier (vers jauge Innovate)

**Composants spécifiés:**
- **PTC 0805:** Bourns MF-MSMF050-2 (0,5A hold, 1A trip)
- **Ferrite 0805:** Murata BLM21PG221SN1 (220Ω @ 100MHz, 2A)
- **TVS:** P6KE18CA (600W, 18V nominal)

**Fonctions:**
1. PTC : Protection I auto-réarmante
2. Ferrite : Filtrage EMI sortie (HF)
3. 100nF : Découplage HF
4. P6KE18CA : Protection surtension locale

_______________________________________________________________________________

## PROGRAMMATION ISP

**Hors circuit avec adaptateur SOIC8→DIP8:**
1. ATtiny85 dans adaptateur SA602 ou équivalent
2. Adaptateur sur breadboard + USBasp
3. Arduino IDE: Tools → Burn Bootloader (fuses 1×)
4. Upload code PWM_V1_5_1.ino
5. Retirer ATtiny, souder sur PCB

**Config Arduino IDE:**
- Board: ATtiny25/45/85 (ATTinyCore)
- Clock: 8 MHz Internal
- BOD: Disabled
- Programmer: USBasp

**Fuses:** lfuse=0xE2, hfuse=0xDF, efuse=0xFF

_______________________________________________________________________________

## TESTS OBLIGATOIRES

### Test 1: Ohmmètre OFF
- +12V_PROT ↔ GND : >100kΩ
- +5V_MCU ↔ GND : >1MΩ (régulateur OFF)

### Test 2: Tensions
- +12V_PROT : 11,7-12,0V
- +5V_MCU : 5,00V ±0,10V ✅

### Test 3: PWM fonctionnel
- PWM 0V : OUT=0V
- PWM 4,8V : OUT=12V ✅
- PWM 10V : OUT=0V

### Test 4: Ripple oscillo
- FILT2 : <20mVpp ✅
- ADC_DIV : <10mVpp ✅

### Test 5: Hystérésis
- Montée ON : 2,84V ±0,2V
- Descente OFF : 2,64V ±0,2V
- Écart : 200mV ±50mV ✅

### Test 6: Sleep mode
- PWM=0V : I_total <0,8mA ✅

### Test 7: Cold-crank 6V
- +5V_MCU stable 5,00V ✅
- Circuit fonctionnel

### Test 8: Load-dump 18V
- Pulse 18V 100ms
- P6KE18CA clamp <20V
- +5V_MCU stable

_______________________________________________________________________________

## BOM V1.5

### Semiconducteurs
| Ref | Composant | Package | Qté |
|-----|-----------|---------|-----|
| U1 | ATtiny85-20SU | SOIC-8 | 1 |
| U2 | MIC5219-5.0YM5-TR | SOT-23-5 | 1 |
| Q1 | BS170 | TO-92 | 1 |
| Q2 | FQP27P06 | TO-220 | 1 |
| D1,D2,D3 | SB2100 | DO-201AD | 3 |
| D4,D5 | BAT85 | DO-35 | 2 |
| D6 | 1N4733A | DO-41 | 1 |
| TVS1,TVS2 | 1.5KE18CA | DO-201 | 2 |
| TVS3 | P6KE18CA | DO-15 | 1 |

### Passifs
| Ref | Composant | Type | Qté |
|-----|-----------|------|-----|
| R1 | 33kΩ ±1% | 0805 1/4W | 1 |
| R2 | 20kΩ ±1% | 0805 1/4W | 1 |
| R3,R4 | 10kΩ ±1% | 0805 1/4W | 4 |
| R5 | 100Ω | 0805 | 1 |
| C1,C2 | 4.7µF film 50V | Radial | 2 |
| C3 | 100µF 50V | Electro | 1 |
| C4-C9 | 100nF X7R | 0805 | 6 |
| C10,C11 | 10µF 16V | 0805 | 4 |
| PTC1 | MF-MSMF050-2 | 0805 | 1 |
| L1 | BLM21PG221SN1 | 0805 | 1 |

### Divers
- Fusible 0.5A rapide
- Borniers 2 positions (entrée/sortie)
- Dissipateur TO-220 (si >200mA)

**Coût estimé:** ~8€ (hors PCB)

_______________________________________________________________________________

## AVANTAGES V1.5

**vs V9.01 analogique:**
- −9 composants (TL431, LM393, BC327, 2N2222A, bootstrap)
- Seuils modifiables software
- Latence 65ms vs 10ms (acceptable @ 108Hz)

**vs V1.4:**
- ✅ Sortie PTC auto-réarmante
- ✅ EMI sortie optimisé (ferrite + 100nF)
- ✅ Protection surtension locale (P6KE18CA)
- ✅ Entrée simplifiée (sans ferrite série)

**Fiabilité équivalente V9.01:**
- Filtrage RC identique (-76dB)
- Sleep <0,5mA
- Protections automotive complètes

_______________________________________________________________________________

## NOTES IMPORTANTES

**Régulateur alternatives:**
- MIC5219-5.0 (SOT-23-5) : compact, Iq 70µA
- LD1117V50 (TO-220) : plus facile à souder, Iq 5mA
- TLV1117-50 (TO-220) : équivalent LD1117

**Ferrite sortie:**
- Murata BLM21PG221SN1 : 220Ω @ 100MHz, 2A
- Alternative : BLM21PG331SN1 (330Ω @ 100MHz)

**PTC 0805:**
- Bourns MF-MSMF050-2 : 0,5A hold, 1A trip, 16V
- Alternative : Littelfuse 0ZCJ0050AF2E

**Version:** V1.5 - Novembre 2025  
**Statut:** Design finalisé, prêt fabrication  
**Auteur:** Mehdi + Claude
