# Bill of Materials (BOM)

Liste complète des composants pour Circuit PWM µC V1.5.

---

## 📦 Fichiers disponibles

- **BOM_V1_5.csv** : Liste détaillée avec références constructeurs
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

### À acheter neufs obligatoirement

| Composant | Référence | Raison |
|-----------|-----------|--------|
| ATtiny85-20SU | ATTINY85-20SU | Contrefaçons fréquentes |
| FQP27P06 | FQP27P06 | Performances RdsON critiques |
| MIC5219 | MIC5219-5.0YM5-TR | Iq 70µA essentiel pour sleep |

### Alternatives acceptables

| Composant | Original | Alternative |
|-----------|----------|-------------|
| Régulateur | MIC5219-5.0 (SOT-23-5) | LD1117V50 (TO-220) |
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
| Régulateur | 7805 (Iq 5mA) | MIC5219 (Iq 70µA) |
| Condo 4.7µF | Céramique X7R | Film polyester/MKT |
| TVS | 1N47xxA (zener) | 1.5KExxCA (TVS) |
| Diode alim | 1N4007 (lente) | SB2100 (Schottky) |

### 🔍 Vérifications réception

1. **ATtiny85** : Vérifier marquage ATTINY85-20SU (pas 85V)
2. **FQP27P06** : Tester RdsOn avec multimètre (doit être <100mΩ)
3. **MIC5219** : Vérifier package SOT-23-5 (pas SOT-23-3)
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

**Version** : 1.0  
**Dernière mise à jour** : 2025-11-13  
**Auteur** : mmmprod
