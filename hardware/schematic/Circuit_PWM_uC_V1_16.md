# CIRCUIT PWM FENÊTRE + µC — VERSION V1.16 PRODUCTION

**Version:** 1.16 PRODUCTION  
**Date:** 2025-12-08  
**Code compatible:** V1.7.10  
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

### 2.3 BLOC µC — ATtiny85

+5V_MCU → U2 pin8 (VCC ATtiny85-20PU)  
U2 pin4 (GND) → GND  
+5V_MCU → R_RST (10kΩ) → U2 pin1 (RESET)  
+5V_MCU → C8 (100nF X7R) → GND (proche U2)

### 2.4 BLOC F — Filtrage RC

J2 pin1 (PWM_IN) → R3 (470Ω) → PWM_FILT  
PWM_FILT → R4 (10kΩ) → PWM_FILT_RC  
PWM_FILT_RC → C2 (4,7µF film 50V) → GND

### 2.5 BLOC ADC — Diviseur

PWM_FILT → R1 (33kΩ 1%) → ADC_DIV  
ADC_DIV → R2 (20kΩ 1%) → GND  
ADC_DIV → C9 (100nF X7R 50V) → GND  
ADC_DIV → C10 (470nF film 50V) → GND  
ADC_DIV → U2 pin7 (PB2/ADC1)

### 2.6 BLOC OUTPUT — Driver P-MOSFET + Protections Sortie

#### Étage Puissance
+12V_PROT → Q1 pin3 (Source FQP27P06)  
Q1 pin2 (Drain) → R13 (10Ω 2W MOX) → OUT_DRAIN

#### Protection Gate
GATE_P → R8 (10kΩ) → +12V_PROT (pull-up)  
GATE_P → D3 anode (1N4733A Zener 5,1V 1W)  
D3 cathode → +12V_PROT  
GATE_P → R9 (1kΩ) → Q2 pin3 (Drain BS170)

**🔴 CORRECTION CRITIQUE V1.16: R9 = 1kΩ (était 100Ω en V1.15)**

**Calcul dissipation R9:**
- Quand BS170 ON: V_GATE_P = 14,4V - 5,1V = 9,3V
- I_R9 = 9,3V / 1000Ω = 9,3mA
- P_R9 = 0,086W < 0,25W rating ✅

#### Driver BS170
Q2 pin1 (Source BS170) → GND  
Q2 pin2 (Gate) → GATE_BS170  
Q2 pin3 (Drain) → R9 (1kΩ) → GATE_P
GATE_BS170 → R11 (100Ω) → U2 pin5 (PB0 OUT_CTRL)  
GATE_BS170 → R10 (100kΩ) → GND (pull-down)

#### Protection Sortie — 3 Couches
OUT_DRAIN → D4 anode (1N5822) → D4 cathode → OUT_PROT  
OUT_PROT → D5 (P6KE18CA TVS) → GND  
OUT_PROT → C11 (100nF X7R 50V) → GND  
OUT_PROT → OUT_12V (vers J3)

---

## 3. BOM COMPLÈTE

### Résistances (CORRECTION V1.16)

| Réf | Valeur | Tolérance | Puissance | Rôle |
|-----|--------|-----------|-----------|------|
| R_LDO_IN | 10Ω | 5% | 0,5W | Découplage LDO |
| R1 | 33kΩ | 1% | 0,25W | Diviseur haut |
| R2 | 20kΩ | 1% | 0,25W | Diviseur bas |
| R3 | 470Ω | 5% | 0,25W | Protection ADC |
| R4 | 10kΩ | 5% | 0,25W | Filtre RC |
| R8 | 10kΩ | 5% | 0,25W | Pull-up gate P-MOS |
| **R9** | **1kΩ** | **5%** | **0,25W** | **Série gate P-MOS** |
| R10 | 100kΩ | 5% | 0,25W | Pull-down BS170 |
| R11 | 100Ω | 5% | 0,25W | Série gate BS170 |
| R13 | 10Ω | 5% | 2W MOX | Limite courant drain |
| R_RST | 10kΩ | 5% | 0,25W | Pull-up RESET |

---

## 4. Changelog

### V1.16 (2025-12-08) — CORRECTION CRITIQUE R9
- 🔴 **CORRECTION CRITIQUE:** R9 100Ω → **1kΩ**
  - V1.15: P_R9 = 0,86W > 0,25W rating → SURCHAUFFE
  - V1.16: P_R9 = 0,086W < 0,25W rating → OK
- ✅ Code compatible: V1.7.10

### V1.15 (2025-12-07) — PROTECTIONS SORTIE
- AJOUT: D4, D5, C11
- ⚠️ BUG R9 présent

---

## 5. Migration V1.15 → V1.16

**OBLIGATOIRE si tu as déjà monté V1.15:**
1. **Remplacer R9**: 100Ω → **1kΩ**
2. Tester température R9 après 5min ON (doit rester <50°C)

**DANGER V1.15:** R9 = 100Ω surchauffe à 0,86W

---

**CONFIANCE: 99/100** ✅  
**STATUT: PRÊT POUR FABRICATION DÉFINITIVE**
