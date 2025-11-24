# CIRCUIT PWM Fenêtre + µC – Version V1.9

0. Contexte et objectif

Application: interface entre le signal PWM 108 Hz du variateur Ford Sync et l’entrée "dimming" de la jauge Innovate (fil blanc), pour ne fournir du +12 V que lorsque le PWM se trouve dans une fenêtre centrale stable. L’objectif est:

- D’éviter les basculements intempestifs en cas de bruit, mouvements rapides du dimmer ou excursions aux extrémités.
- De ne jamais exposer l’ATtiny85 ou la jauge à des surtensions dangereuses.
- De rester compatible avec le code 1.6.3 et les seuils ADC documentés (fenêtre ON stable 3,10 V à 6,61 V).

Le montage repose sur:
- Une protection entrée +12 V: fusible, diode Schottky, TVS.
- Un régulateur NCV2931AD-5.0R2G pour générer +5V_MCU robuste automotive.
- Un ATtiny85 (8 MHz, BOD 2,7 V) qui lit le PWM filtré sur ADC1.
- Un driver BS170 + P-MOS FQP27P06 pour commuter le +12 V logique vers la jauge.
- Une limitation de courant série (R13 = 10 Ω 2 W) sur la sortie.

1. Nœuds principaux

Nœuds logiques à respecter:

- +BATT : +12 V véhicule, avant fusible carte.
- FUSE_OUT : Sortie de F1, toujours au plus proche du bornier J1.
- +12V_PROT : +12 V protégé après F1, D1, TVS. Alimente le reste du circuit.
- +12V_LDO_IN : +12 V vu par le LDO après R_LDO_IN (10 Ω).
- +5V_MCU : +5 V régulé par NCV2931, alimente exclusivement l’ATtiny85 et le petit signal.
- PWM_IN : Entrée PWM brutes depuis le Sync (J2 pin1).
- PWM_FILT : Nœud après R3, avant R4/C2.
- PWM_FILT_RC : Nœud après filtrage RC R4 + C2.
- ADC_DIV : Nœud de mesure ADC après diviseur R1/R2 et C9/C10.
- OUT_CTRL : Sortie logique de l’ATtiny85 (PB0), commande de BS170.
- GATE_P : Gate du P‑MOS FQP27P06, pilotée par BS170.
- OUT_12V : Sortie logique +12 V vers la jauge (fil blanc Innovate), après R13.

Par défaut, circuit au repos:
- OUT_CTRL = LOW.
- BS170 OFF.
- GATE_P tirée au +12V_PROT via R8.
- P‑MOS OFF.
- OUT_12V ≈ 0 V.

2. Blocs et connexions détaillées

2.1 Bloc A – Protection entrée +12 V

Connexions:
- J1 Pin1 (+BATT) → F1 (fusible 0,5 A rapide) → nœud FUSE_OUT.
- FUSE_OUT → D1 (1N5822 Schottky) anode sur FUSE_OUT, cathode sur +12V_PROT.
- +12V_PROT → D2 (TVS 1.5KE18CA bidirectionnelle) → GND.
- +12V_PROT → C4 (100 µF 50 V) → GND.
- +12V_PROT → C5 (100 nF X7R 50 V) → GND.

Commentaires:
- F1 protège la voiture en cas de défaut interne.
- D1 protège contre l’inversion de polarité et chute légèrement la tension.
- D2 écrête les surtensions de type load‑dump.
- C4 et C5 stabilisent localement le +12 V protégé.

2.2 Bloc B – Régulateur 5 V NCV2931

Connexions:
- +12V_PROT → R_LDO_IN = 10 Ω → +12V_LDO_IN.
- +12V_LDO_IN → Pin 3 (INPUT) NCV2931AD-5.0R2G.
- Pins GND du NCV2931 → plan de masse.
- OUTPUT → +5V_MCU.

Découplages:
- C_IN = 100 nF X7R 50 V proche pin IN.
- C6 = 10 µF électrolytique 16/25 V sur +5V_MCU.
- C7 = 100 nF X7R 50 V proche ATtiny.

Rappels NCV2931:
- Vout = 5,0 V ±3,8 %.
- Iout max ≈ 100 mA.
- Dropout < 0,6 V @ 100 mA.
- Iq typ. quelques mA, robuste automotive.

Remarques:
- R_LDO_IN = 10 Ω limite les pointes de courant lors de transitoires et répartit l’énergie avec la TVS.
- Toutes les pins GND du LDO doivent être reliées ensemble et au plan de masse.

2.3 Bloc µC – ATtiny85

Connexions:
- +5V_MCU → ATtiny85 Pin 8 (VCC).
- ATtiny85 Pin 4 (GND) → plan de masse.
- +5V_MCU → R_RST = 10 kΩ → ATtiny85 Pin 1 (RESET).
- C8 = 100 nF X7R 50 V proche VCC/GND.

Fuses attendues:
- Horloge interne 8 MHz.
- BOD = 2,7 V (efuse 0xFD).
- lfuse = 0xE2, hfuse = 0xDF, efuse = 0xFD.

2.4 Bloc F – Filtrage RC du PWM

Connexions:
- J2 Pin1 PWM_IN → R3 = 470 Ω → PWM_FILT.
- PWM_FILT → R4 = 10 kΩ → PWM_FILT_RC.
- PWM_FILT_RC → C2 = 4,7 µF film 50 V → GND.

Commentaires:
- R3 limite le courant d’injection dans l’ADC en cas de dépassement de VCC.
- R4 + C2 réalisent le lissage du PWM 108 Hz.
- τ ≈ 49,2 ms, fc ≈ 3,2 Hz, atténuation ≈ −30 dB à 108 Hz, ripple < 50 mVpp.

2.5 Bloc ADC – Diviseur et anti‑aliasing

Connexions:
- PWM_FILT → R1 = 33 kΩ (1 %) → ADC_DIV.
- ADC_DIV → R2 = 20 kΩ (1 %) → GND.
- ADC_DIV → C9 = 100 nF X7R → GND.
- ADC_DIV → C10 = 470 nF film → GND.
- ADC_DIV → ATtiny85 Pin 7 (PB2 / ADC1).

Rappels:
- k = 20k/(33k+20k) ≈ 0,377.
- 1 count ADC ≈ 4,888 mV ADC → ≈ 12,95 mV sur V_PWM.

2.6 Bloc Output – Driver P‑MOSFET

Topologie:
- +12V_PROT → Q1 (FQP27P06) Source.
- Q1 Drain → R13 = 10 Ω 2 W → OUT_12V.
- Q1 Gate → GATE_P.

Gate protection/pull:
- GATE_P → R8 = 10 kΩ → +12V_PROT.
- GATE_P → D3 cathode (1N4733A Zener 5,1 V) → anode → +12V_PROT.
- GATE_P → R9 = 100 Ω → Q2 Drain (BS170).

Driver BS170:
- Q2 Source → GND.
- Q2 Gate (GATE_BS170) via R11 = 100 Ω ← OUT_CTRL (PB0).
- GATE_BS170 → R10 = 100 kΩ → GND (pull‑down).

Logique:
- OUT_CTRL LOW → BS170 OFF → GATE_P pulled to +12V_PROT → P‑MOS OFF → OUT_12V ≈ 0 V.
- OUT_CTRL HIGH → BS170 ON → GATE_P ≈ 0 V → P‑MOS ON → OUT_12V ≈ +12V_PROT (limité par R13).

2.7 Bloc J – Connecteurs

- J1: Alim (Pin1 +BATT via F1, Pin2 GND).
- J2: PWM (Pin1 PWM_IN, Pin2 GND).
- J3: Sortie jauge (Pin1 OUT_12V, Pin2 NC). NE PAS RELIER GND via J3 (utiliser faisceau jauge).

3. Seuils ADC et logique fenêtre

Seuils (counts / V_PWM):

- SEUIL_BAS = 219 → V_PWM ≈ 2,84 V.
- SEUIL_BAS_ON = 239 → V_PWM ≈ 3,10 V.
- SEUIL_BAS_LIMITE = 199 → V_PWM ≈ 2,58 V.
- SEUIL_HAUT_LIMITE = 511 → V_PWM ≈ 6,61 V.
- SEUIL_HAUT_ON = 551 → V_PWM ≈ 7,15 V.

Résumé:
- Fenêtre ON stable: 3,10 V → 6,61 V (239–511 counts).
- Hystérésis basse: 199–239 (2,58 V–3,10 V).
- Hystérésis haute: 511–551 (6,61 V–7,15 V).

4. BOM synthétique

(Semiconducteurs, résistances, condensateurs et connectique listés — voir hardware/bom/BOM_V1_9.csv pour version CSV détaillée.)

5. Points de contrôle "premortem"

Avant tests: vérification orientation D1/D2/D3, position F1, continuités, câblage LDO, connexions ADC, pinout BS170 et FQP27P06, RESET pull‑up, vérifier R3/R4/R1/R2/C9/C10.

Code chargé: version 1.6.3 (SEUIL_BAS_ON = 239, SEUIL_HAUT_LIMITE = 511, SEUIL_HAUT_ON = 551, SEUIL_BAS_LIMITE = 199).
