# CIRCUIT PWM FENÊTRE + µC — VERSION V1.11

## 0. Contexte et Objectif

Application: Interface PWM 108Hz Ford Sync → Jauge Innovate (fil blanc)
Sortie +12V uniquement dans fenêtre PWM centrale stable (3,10V à 6,61V)

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
| OUT_12V | Sortie vers jauge |

## 5. Programmation ATtiny85

Méthode: ATtiny retiré, programmé sur adaptateur ISP externe, puis réinstallé en DIP-8.
Pas de connecteur J4 ISP sur circuit final.

Code compatible: V1.7.5 (recommandé) ou V1.7.4+

## 7. Changelog

### V1.11 (2025-12-05)
- Code compatible: V1.7.5 (ADC désactivé pendant sleep)
- Consommation sleep réduite: ~0,65mA (vs ~0,7mA V1.10)
- Documentation uniquement, hardware identique à V1.10

### V1.10 (2025-12-05)
- Correction orientation D3 (anode→GATE_P, cathode→+12V_PROT)
- Audit PREMORTEM V3.5 complet
- Validation KB_ANALOG_MASTER V2.6
