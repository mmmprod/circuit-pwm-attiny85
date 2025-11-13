# PCB Design - Circuit PWM µC V1.5

Recommandations pour conception PCB.

---

## 📐 Spécifications PCB

### Format
- **Taille** : ~50×40mm recommandé
- **Couches** : 2 couches (Top + Bottom)
- **Épaisseur** : 1.6mm standard
- **Finition** : HASL (sans plomb) ou ENIG

### Pistes
- **Alim 12V** : 1.0mm min (500mA max)
- **Sortie OUT** : 1.0mm min (500mA max)
- **Signaux** : 0.3mm min
- **GND** : Plan masse complet bottom layer

### Clearance
- **Piste-piste** : 0.25mm min
- **Piste-pad** : 0.25mm min
- **Haute tension** : 1.0mm (12V isolé du 5V)

---

## 🔧 Layout recommandations

### Placement composants
[ENTRÉE] [ALIM] [MCU] [OUTPUT] [SORTIE] J1 → TVS1 → U2 → ATtiny85 → BS170 → FQP27P06 → J3 J2 → TVS2 → Filtrage RC ↓ PTC/Ferrite TVS3

### Zones critiques

#### 1. Alimentation (U2 régulateur)
- ✅ C10 (10µF) **≤10mm** de pin IN
- ✅ C11 (10µF) **≤10mm** de pin OUT
- ✅ C6, C7 (100nF) au plus près des pins
- ✅ Pistes courtes et larges (1mm)

#### 2. MCU (U1 ATtiny85)
- ✅ C12, C13 (10µF + 100nF) **≤10mm** de pins Vcc/GND
- ✅ Via GND sous le composant si possible
- ✅ Entrée ADC (pin 7) éloignée des pistes digitales

#### 3. Sortie (Q2 P-MOSFET)
- ✅ Piste drain large (1mm)
- ✅ PTC1 + L1 au plus près du drain
- ✅ TVS3 entre sortie et GND (court)
- ✅ Dissipateur prévu si >200mA (pad TO-220)

#### 4. Filtrage PWM
- ✅ R1-C1-R2-C2 en ligne continue
- ✅ Éviter boucles de masse
- ✅ GND côté cold (entrée DIM_IN)

---

## 🌡️ Thermique

### Dissipation

| Composant | Dissipation | Recommandation |
|-----------|-------------|----------------|
| FQP27P06 | 0.1W @ 100mA | Pad cuivre 20×20mm |
| FQP27P06 | 0.5W @ 300mA | Dissipateur TO-220 |
| U2 régulateur | 0.5W @ 100mA | Pad cuivre 15×15mm |
| ATtiny85 | <50mW | Aucune |

### Zones thermiques
- ✅ Pad cuivre sous Q2 (FQP27P06)
- ✅ Vias thermiques (Ø0.5mm, ×4 min)
- ✅ Plan GND bottom pour dissipation

---

## 🔌 Connecteurs

### J1 - Alimentation
Pin 1: +12V_ACC (rouge) Pin 2: GND (noir)
**Type** : Bornier à vis 5mm, 2 positions

### J2 - Entrée PWM
Pin 1: DIM_IN (signal) Pin 2: GND (référence)
**Type** : Bornier à vis 5mm, 2 positions

### J3 - Sortie
Pin 1: OUT+ (vers jauge) Pin 2: GND (retour)
**Type** : Bornier à vis 5mm, 2 positions

### Alternative : Headers 2.54mm (prototypage)

---

## ⚡ Plan de masse

### Structure recommandée
TOP LAYER:

Signaux + alimentation
Pas de plan masse (sauf zones locales)
BOTTOM LAYER:

Plan masse complet (GND)
Pistes alim si nécessaire

### Points de connexion GND
1. ✅ GND alim (J1.2) → Plan masse
2. ✅ GND régulateur → Plan masse (via court)
3. ✅ GND MCU → Plan masse (via sous composant)
4. ✅ GND sortie (J3.2) → Plan masse
5. ✅ TVS anodes → Plan masse (via multiples)

---

## 🧪 Prototypage

### Option 1 : Breadboard (recommandé pour tests)
- Utiliser modules DIP :
  - ATtiny85 → Adaptateur SOIC8→DIP8
  - MIC5219 → LD1117V50 (TO-220, plus facile)
  - Résistances/condensateurs traversants

### Option 2 : PCB prototype (perfboard)
- Prévoir trous 2.54mm (0.1")
- Souder fils pour pistes
- Attention isolation haute tension (12V)

### Option 3 : PCB pro (production)
- KiCad, Eagle, EasyEDA
- Fabrication JLCPCB, PCBWay (5 PCB ~2€)
- Délai 1-2 semaines

---

## 📦 Fichiers PCB (à venir)

- [ ] Schématique KiCad (.kicad_sch)
- [ ] Layout KiCad (.kicad_pcb)
- [ ] Gerbers (fabrication)
- [ ] 3D render (.step)
- [ ] BoM export KiCad

**Contributeurs bienvenus** pour design PCB !

---

## 🎨 Sérigraphie recommandée

### Top silkscreen
Circuit PWM µC V1.5 github.com/mmmprod/circuit-pwm-attiny85

J1: +12V GND J2: PWM GND J3: OUT+ GND

⚠️ 6-16V DC ONLY

### Repères polarité
- ✅ **+** près pin +12V (J1.1)
- ✅ **~** près pin PWM (J2.1)
- ✅ **OUT** près pin sortie (J3.1)

---

## ⚠️ Checklist pré-fabrication
☐ Schématique validé (pas d'erreurs DRC) 
☐ Footprints vérifiés (ATtiny85 SOIC-8, MIC5219 SOT-23-5) 
☐ Clearance haute tension OK (1mm min) 
☐ Plan masse continu (pas d'îlots isolés) 
☐ Vias thermiques sous FQP27P06 
☐ Condensateurs découplage ≤10mm des ICs 
☐ Trous de montage (M3, 4× coins) 
☐ Sérigraphie lisible (texte ≥1mm) 
☐ Numéro version PCB (ex: "V1.5 - 2025-11")

---

## 🔗 Ressources

- **KiCad** : https://www.kicad.org/ (gratuit, open-source)
- **EasyEDA** : https://easyeda.com/ (en ligne, gratuit)
- **JLCPCB** : https://jlcpcb.com/ (fabrication PCB économique)
- **PCBWay** : https://www.pcbway.com/
- **SnapEDA** : https://www.snapeda.com/ (librairies composants)

---

**Version** : 1.0  
**Dernière mise à jour** : 2025-11-13  
**Auteur** : mmmprod
