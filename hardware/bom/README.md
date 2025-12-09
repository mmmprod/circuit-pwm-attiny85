# Bill of Materials (BOM)

Liste complète des composants pour Circuit PWM µC V1.18.

---

## 📦 Fichiers disponibles

- **BOM_V1_18.csv** : Liste détaillée avec références constructeurs
- Format CSV compatible Excel/LibreOffice/KiCad

---

## 💰 Coût estimé

| Catégorie | Prix unitaire (€) |
|-----------|-------------------|
| Semiconducteurs | 4.90 |
| Passifs | 2.25 |
| Protection | 1.20 |
| Connecteurs | 0.90 |
| Divers | 0.40 |
| **Total composants** | **9.65 €** |
| PCB (estimation) | 5.00 € |
| **Total projet** | **~15 €** |

Prix indicatifs pour quantité 1-10 pièces (novembre 2025).

---

## 🛒 Fournisseurs recommandés

### France/Europe
- **Mouser** : https://www.mouser.fr
- **Farnell** : https://fr.farnell.com
- **Digikey** : https://www.digikey.fr
- **TME** : https://www.tme.eu (prix compétitifs)

### Alternative économique
- **LCSC** : https://www.lcsc.com (Chine, délai 2-3 semaines)
- **AliExpress** : Composants génériques (vérifier authenticité)

---

## 🔧 Composants critiques

### 🔴 COMPOSANTS V1.18 : D6, R13, C4, C6

| Composant | Valeur V1.18 | Valeur V1.17 | Raison |
|-----------|--------------|--------------|--------|
| **D6** | **P6KE15CA** | Absent | Protection ESD entrée PWM |
| **R13** | **10Ω 5W MOX** | 10Ω 2W | Marge sécurité court-circuit ×4 |
| **C4** | **100µF 105°C 5000h** | 100µF 85°C | Durabilité automotive |
| **C6** | **10µF 105°C 5000h** | 10µF 85°C | Durabilité automotive |

**Références électrolytiques 105°C recommandées:**
- C4: Nichicon UHE1H101MPD ou équivalent
- C6: Nichicon UHE1E100MDD ou équivalent

### 🔴 COMPOSANT CRITIQUE : R9

| Composant | Valeur | Raison |
|-----------|--------|--------|
| **R9** | **1kΩ** (1/4W, 1%) | **CRITIQUE** : Limitation courant Zener D3 |

**⚠️ ATTENTION V1.15** : Bug R9=100Ω causait surchauffe (0,86W > 0,25W rating)

**V1.16 CORRIGÉ** : R9=1kΩ → 9,3mA → 0,086W < 0,25W ✅

**Vérification obligatoire** :
- Mesurer R9 à l'ohmmètre : doit être **~1kΩ** (pas 100Ω)
- Test température : R9 < 50°C après 5min fonctionnement
- Protocole V9.28 Phase 13 : validation température

### 🟡 COMPOSANTS V1.17 : C12 et R11

| Composant | Valeur V1.17 | Valeur V1.16 | Raison |
|-----------|--------------|--------------|--------|
| **C12** | **10nF X7R 50V** | Absent | Protection EMI entrée J2 |
| **R11** | **1kΩ** | 100Ω | Défense profondeur gate BS170 |

**Placement C12** : Directement sur bornier J2, entre pin1 (PWM_IN) et pin2 (GND)

**Note R11** : Limite courant si BS170 claque en court-circuit D-G

### À acheter neufs obligatoirement

| Composant | Référence | Raison |
|-----------|-----------|--------|
| ATtiny85-20SU | ATTINY85-20SU | Contrefaçons fréquentes |
| FQP27P06 | FQP27P06 | Performances RdsON critiques |
| NCV2931 | NCV2931AD-5.0R2G | Régulateur 5V robuste automotive |

### Alternatives acceptables

| Composant | Original | Alternative |
|-----------|----------|-------------|
| Régulateur | NCV2931-5.0 (TSOP-5) | LD1117V50 (TO-220) |
| TVS alim | 1.5KE18CA | 1.5KE20CA (marge) |
| PTC | MF-MSMF050-2 | 0ZCJ0050AF2E |
| Ferrite | BLM21PG221SN1 | BLM21PG331SN1 |

---

## 📝 Notes d'achat

### Condensateurs
- **100nF** : X7R obligatoire (pas Y5V, instable en température)
- **10µF** : X5R ou X7R, tension ≥16V
- **4.7µF** : Film polyester/MKT (pas céramique, ESR trop faible)
- **100µF** : Électrolytique low-ESR, 105°C si possible

### Résistances
- Tolérance **1%** obligatoire pour R1-R4 (précision ADC)
- Puissance 1/4W suffisante (dissipation <50mW max)

### Diodes
- SB2100 : Schottky **obligatoire** (Vf faible)
- BAT85 : Schottky rapide pour clamps ESD
- 1N4733A : Zener 1W (pas 1/2W, dissipation gate P-MOS)

### MOSFETs
- BS170 : Vérifier origine (TO-92 authentique)
- FQP27P06 : **Attention contrefaçons**, acheter distributeur agréé

---

## ⚠️ Pièges à éviter

### ❌ Composants incompatibles

| Composant | ❌ Ne pas utiliser | ✅ Utiliser |
|-----------|-------------------|-------------|
| Régulateur | 7805 (Iq 5mA) | NCV2931 (Iq optimisé) |
| Condo 4.7µF | Céramique X7R | Film polyester/MKT |
| TVS | 1N47xxA (zener) | 1.5KExxCA (TVS) |
| Diode alim | 1N4007 (lente) | SB2100 (Schottky) |

### 🔍 Vérifications réception

1. **ATtiny85** : Vérifier marquage ATTINY85-20SU (pas 85V)
2. **FQP27P06** : Tester RdsOn avec multimètre (doit être <100mΩ)
3. **NCV2931** : Vérifier package TSOP-5 correct
4. **Condensateurs** : Vérifier tension nominale gravée

---

## 📦 Quantités recommandées

Pour fabrication 5 circuits + prototypage :

| Composant | Qté unitaire | Qté totale | Stock recommandé |
|-----------|--------------|------------|------------------|
| ATtiny85 | 1 | 5 | +2 (backup) |
| FQP27P06 | 1 | 5 | +2 (backup) |
| Résistances | 7 | 35 | +10 (assortiment) |
| Condensateurs 100nF | 6 | 30 | +20 (usage fréquent) |
| TVS | 3 | 15 | +5 (backup) |

**Conseil** : Commander lots de 10 pour composants <0.50€ (économie port).

---

## 🔗 Liens utiles

- **Mouser BOM Tool** : Importer CSV pour commande directe
- **Octopart** : Comparateur prix multi-fournisseurs
- **FindChips** : Recherche disponibilité stock

---

**Version** : 2.0  
**Dernière mise à jour** : 2025-12-09  
**Auteur** : mmmprod
