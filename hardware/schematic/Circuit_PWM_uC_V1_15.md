# CIRCUIT PWM FENÊTRE + µC — VERSION V1.15 FINALE

**Version:** 1.15 PRODUCTION  
**Date:** 2025-12-07  
**Code compatible:** V1.7.8  
**Statut:** ✅ VALIDÉ POUR FABRICATION DÉFINITIVE

---

## 0. Contexte et Objectif

Application: Interface PWM 108Hz Ford Sync → Jauge Innovate (fil blanc)  
Sortie +12V uniquement dans fenêtre PWM centrale stable (3,10V à 6,61V)

---

## 1. Nœuds Principaux

| Nœud | Description |
|------|-------------|
| +BATT | +12V véhicule avant fusible |
| FUSE_OUT | Sortie F1 |
| +12V_PROT | +12V protégé après D1, D2 |
| +12V_LDO_IN | Entrée LDO après R_LDO_IN |
| +5V_MCU | Sortie régulateur 5V |
| PWM_IN | Entrée PWM brute (J2) |
| PWM_FILT | Après R3 |
| PWM_FILT_RC | Après R4+C2 |
| ADC_DIV | Point mesure ADC |
| OUT_CTRL | Sortie ATtiny85 (PB0) |
| GATE_P | Gate FQP27P06 |
| OUT_DRAIN | Sortie drain FQP27P06 après R13 |
| OUT_PROT | Sortie après D4 anti-backfeed |
| OUT_12V | Sortie finale vers jauge (J3) |

---

## 2. Blocs et Connexions

### 2.1 BLOC A — Protection Entrée +12V

J1 pin1 (+BATT) → F1 (0,5A Fast-blow) → FUSE_OUT  
FUSE_OUT → D1 anode (1N5822 Schottky)  
D1 cathode → +12V_PROT  
+12V_PROT → D2 (1.5KE18CA TVS bidirectionnelle) → GND  
+12V_PROT → C4 (100µF 50V électro) → GND  
+12V_PROT → C5 (100nF X7R 50V) → GND

### 2.2 BLOC B — Régulateur NCV2931

+12V_PROT → R_LDO_IN (10Ω 0,5W) → +12V_LDO_IN  
+12V_LDO_IN → U1 pin3 (INPUT NCV2931AD-5.0R2G)  
U1 pins 2,5 (GND) → GND  
U1 pins 6,7,8 (OUTPUT) → +5V_MCU  
+12V_LDO_IN → C_IN (100nF X7R 50V) → GND  
+5V_MCU → C6 (10µF électro 25V) → GND  
+5V_MCU → C7 (100nF X7R 50V) → GND

Specs NCV2931:
- Vin_max: 26V (load-dump OK)
- Vout: 5,0V ±3,8%
- Iout: 100mA max
- Dropout: <0,6V @ 100mA
- Iq: 0,4mA typ

### 2.3 BLOC µC — ATtiny85

+5V_MCU → U2 pin8 (VCC ATtiny85-20PU)  
U2 pin4 (GND) → GND  
+5V_MCU → R_RST (10kΩ) → U2 pin1 (RESET)  
+5V_MCU → C8 (100nF X7R) → GND (proche U2)

Fuses:
- lfuse = 0xE2 (8MHz interne)
- hfuse = 0xDF (SPI enabled)
- efuse = 0xFD (BOD 2,7V)

### 2.4 BLOC F — Filtrage RC

J2 pin1 (PWM_IN) → R3 (470Ω) → PWM_FILT  
PWM_FILT → R4 (10kΩ) → PWM_FILT_RC  
PWM_FILT_RC → C2 (4,7µF film 50V) → GND

Calculs:
- τ = R4 × C2 = 47ms
- fc = 3,4Hz
- Atténuation @ 108Hz: -30dB

### 2.5 BLOC ADC — Diviseur

PWM_FILT → R1 (33kΩ 1%) → ADC_DIV  
ADC_DIV → R2 (20kΩ 1%) → GND  
ADC_DIV → C9 (100nF X7R 50V) → GND  
ADC_DIV → C10 (470nF film 50V) → GND  
ADC_DIV → U2 pin7 (PB2/ADC1)

Calculs:
- k = R2/(R1+R2) = 0,377
- V_ADC = V_PWM × 0,377
- 1 count = 12,95mV PWM

### 2.6 BLOC OUTPUT — Driver P-MOSFET + Protections Sortie

Q1 FQP27P06 (TO-220):
- Pin1 = Gate = GATE_P
- Pin2 = Drain → R13 → sortie
- Pin3 = Source → +12V_PROT

#### Étage Puissance
+12V_PROT → Q1 pin3 (Source FQP27P06)  
Q1 pin2 (Drain) → R13 (10Ω 2W MOX) → OUT_DRAIN

#### Protection Gate
GATE_P → R8 (10kΩ) → +12V_PROT (pull-up)  
GATE_P → D3 anode (1N4733A Zener 5,1V 1W)  
D3 cathode → +12V_PROT  
GATE_P → R9 (100Ω) → Q2 pin3 (Drain BS170)

**Orientation D3:** Anode vers GATE_P, bague (cathode) vers +12V_PROT

#### Driver BS170
Q2 pin1 (Source BS170) → GND  
Q2 pin2 (Gate) → GATE_BS170  
Q2 pin3 (Drain) → R9 → GATE_P  
GATE_BS170 → R11 (100Ω) → U2 pin5 (PB0 OUT_CTRL)  
GATE_BS170 → R10 (100kΩ) → GND (pull-down)

Pinout BS170 (TO-92): pin1=Source, pin2=Gate, pin3=Drain

#### 🔴 PROTECTION SORTIE — 3 COUCHES (NOUVEAU V1.15)

**Couche 1 — Anti-backfeed Schottky:**  
OUT_DRAIN → D4 anode (1N5822 Schottky 40V 3A)  
D4 cathode → OUT_PROT

**Couche 2 — TVS surtension sortie:**  
OUT_PROT → D5 (P6KE18CA TVS 18V 600W bidirectionnelle) → GND

**Couche 3 — Filtrage EMI:**  
OUT_PROT → C11 (100nF X7R 50V) → GND  
OUT_PROT → OUT_12V (vers J3)

#### Logique
- OUT_CTRL=LOW → BS170 OFF → GATE_P=+12V (R8) → P-MOS OFF → OUT_12V≈0V
- OUT_CTRL=HIGH → BS170 ON → GATE_P≈6,9V (clamp D3) → P-MOS ON → OUT_12V≈+11V

### 2.7 BLOC J — Connecteurs

J1 (Alimentation): Pin1: +BATT → F1, Pin2: GND  
J2 (Entrée PWM): Pin1: PWM_IN → R3, Pin2: GND  
J3 (Sortie Jauge): Pin1: OUT_12V, Pin2: GND

---

## 3. Seuils ADC

| Paramètre | Counts | V_ADC | V_PWM |
|-----------|--------|-------|-------|
| SEUIL_BAS | 219 | 1,07V | 2,84V |
| SEUIL_BAS_ON | 239 | 1,17V | 3,10V |
| SEUIL_BAS_LIMITE | 199 | 0,97V | 2,58V |
| SEUIL_HAUT_LIMITE | 511 | 2,50V | 6,61V |
| SEUIL_HAUT_ON | 551 | 2,69V | 7,15V |

Fenêtre ON: 3,10V à 6,61V (239 à 511 counts)  
Hystérésis: ±520mV symétrique

---

## 4. BOM COMPLÈTE

### Semiconducteurs

| Réf | Composant | Valeur | Package | Rôle |
|-----|-----------|--------|---------|------|
| U1 | NCV2931AD-5.0R2G | LDO 5V 100mA | SOIC-8 | Régulateur |
| U2 | ATtiny85-20PU | µC 8-bit | DIP-8 | Microcontrôleur |
| Q1 | FQP27P06 | P-MOS -60V 27A | TO-220 | Switch haute-côté |
| Q2 | BS170 | N-MOS 60V 0,5A | TO-92 | Driver gate |
| D1 | 1N5822 | Schottky 40V 3A | DO-201AD | Anti-inversion entrée |
| D2 | 1.5KE18CA | TVS 18V 1500W | DO-201AE | Protection entrée |
| D3 | 1N4733A | Zener 5,1V 1W | DO-41 | Clamp gate |
| **D4** | **1N5822** | **Schottky 40V 3A** | **DO-201AD** | **Anti-backfeed sortie** |
| **D5** | **P6KE18CA** | **TVS 18V 600W** | **DO-201AE** | **Protection sortie** |

### Résistances

| Réf | Valeur | Tolérance | Puissance | Rôle |
|-----|--------|-----------|-----------|------|
| R_LDO_IN | 10Ω | 5% | 0,5W | Découplage LDO |
| R1 | 33kΩ | 1% | 0,25W | Diviseur haut |
| R2 | 20kΩ | 1% | 0,25W | Diviseur bas |
| R3 | 470Ω | 5% | 0,25W | Protection ADC |
| R4 | 10kΩ | 5% | 0,25W | Filtre RC |
| R8 | 10kΩ | 5% | 0,25W | Pull-up gate P-MOS |
| R9 | 100Ω | 5% | 0,25W | Série gate P-MOS |
| R10 | 100kΩ | 5% | 0,25W | Pull-down BS170 |
| R11 | 100Ω | 5% | 0,25W | Série gate BS170 |
| R13 | 10Ω | 5% | 2W MOX | Limite courant |
| R_RST | 10kΩ | 5% | 0,25W | Pull-up RESET |

### Condensateurs

| Réf | Valeur | Type | Tension | Rôle |
|-----|--------|------|---------|------|
| C_IN | 100nF | X7R | 50V | Découplage LDO |
| C2 | 4,7µF | Film | 50V | Filtre RC |
| C4 | 100µF | Électro | 50V | Réserve entrée |
| C5 | 100nF | X7R | 50V | Découplage entrée |
| C6 | 10µF | Électro | 25V | Réserve +5V |
| C7 | 100nF | X7R | 50V | Découplage +5V |
| C8 | 100nF | X7R | 50V | Découplage µC |
| C9 | 100nF | X7R | 50V | Filtre ADC 1 |
| C10 | 470nF | Film | 50V | Filtre ADC 2 |
| **C11** | **100nF** | **X7R** | **50V** | **EMI sortie** |

### Connectique

| Réf | Description |
|-----|-------------|
| F1 | Fusible 0,5A Fast-blow 5×20mm |
| J1 | Bornier 2 pins 5mm 15A |
| J2 | Bornier 2 pins 5mm 5A |
| J3 | Bornier 2 pins 5mm 5A |

---

## 5. Schéma Bloc

```
+BATT → F1 → D1 → +12V_PROT ─┬→ R_LDO → NCV2931 → +5V_MCU → ATtiny85
                              │                                  │ PB0
                              │                          R11 → BS170 → GND
                              │                                  │
                              ↓                          R9 ←────┘
                         D2 (TVS)                                │
                              │                          ┌───────┴─── GATE_P
                              ↓                          │       │
                             GND                         D3      R8
                                                         │       │
                                                    +12V_PROT ←──┘
                                                         │
                                                    Q1 (FQP27P06)
                                                         │ Drain
                                                        R13
                                                         │
                                                    OUT_DRAIN
                                                         │
                                                    D4 (1N5822) ← Anti-backfeed
                                                         │
                                                    OUT_PROT
                                                         │
                                             ┌───────────┼───────────┐
                                             │           │           │
                                        D5 (TVS)       C11       OUT_12V
                                             │           │           │
                                            GND         GND         J3
```

---

## 6. Changelog

### V1.15 (2025-12-07) — VERSION FINALE
- 🔴 **AJOUT:** D4 (1N5822) anti-backfeed sortie
- 🔴 **AJOUT:** D5 (P6KE18CA) TVS protection sortie
- 🔴 **AJOUT:** C11 (100nF) filtrage EMI sortie
- ✅ Code compatible: V1.7.8

### V1.14 (2025-12-06)
- Code V1.7.8: Protections défensives firmware

### V1.10 (2025-12-05)
- Correction orientation D3 Zener

---

**STATUT: PRÊT POUR FABRICATION DÉFINITIVE** ✅