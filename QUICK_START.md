# 🚀 DÉMARRAGE RAPIDE - S.I.M.S.

## ⚡ En 3 Étapes

### Étape 1: Ouvrir Wokwi
🔗 **Aller sur:** https://wokwi.com/projects/new/esp32

### Étape 2: Copier les Fichiers

#### 📄 sketch.ino
1. Ouvrir le fichier `sketch.ino` de ce projet
2. Copier tout le contenu (Ctrl+A, Ctrl+C)
3. Dans Wokwi, supprimer le code existant
4. Coller le code (Ctrl+V)

#### 📐 diagram.json
1. Dans Wokwi, cliquer sur "diagram.json" (onglet en haut)
2. Supprimer le contenu
3. Copier le contenu du fichier `diagram.json` de ce projet
4. Coller dans Wokwi

#### 📚 libraries.txt
1. Dans Wokwi, cliquer sur le bouton "+" pour ajouter un fichier
2. Nommer le fichier: `libraries.txt`
3. Copier le contenu du fichier `libraries.txt` de ce projet
4. Coller et sauvegarder

### Étape 3: Lancer !
1. Cliquer sur le bouton vert **▶ Start Simulation**
2. Ouvrir le Serial Monitor (icône 💬 en bas)
3. Observer l'initialisation
4. Commencer à tester ! 🎉

---

## 🎮 Premiers Tests

### Test 1: Voir le Menu
Dans le Serial Monitor, taper:
```
MENU
```

### Test 2: Vérifier l'État
```
STATUS
```

### Test 3: Déclencher une Alarme
1. Cliquer sur le capteur DHT22
2. Mettre température à 35°C
3. Observer l'alarme !

---

## 📁 Structure des Fichiers

```
Mini Projet/
├── sketch.ino              ← Code principal (copier dans Wokwi)
├── diagram.json            ← Schéma circuit (copier dans Wokwi)
├── libraries.txt           ← Bibliothèques (copier dans Wokwi)
├── README.md               ← Documentation complète
├── GUIDE_WOKWI.md         ← Guide détaillé Wokwi
├── SCENARIOS_TEST.md      ← 12 scénarios de test
├── RAPPORT_EXEMPLE.md     ← Template de rapport
└── QUICK_START.md         ← Ce fichier
```

---

## 🎯 Commandes Essentielles

| Commande | Fonction |
|----------|----------|
| `MENU` | Afficher le menu |
| `STATUS` | Voir l'état actuel |
| `SETTEMP 35` | Température max = 35°C |
| `SETHUM 80` | Humidité max = 80% |
| `SETGAS 2500` | Gaz max = 2500 |
| `SETLIGHT 300` | Lumière min = 300 |
| `RESET` | Réinitialiser |

---

## 📊 Seuils par Défaut

- 🌡️ Température max: **30°C**
- 💧 Humidité max: **70%**
- ☁️ Gaz max: **2000**
- 💡 Lumière min: **500**

---

## ✅ Checklist Mini-Projet

### Requis
- [x] ESP32 utilisé
- [x] 2+ capteurs (on a 3: DHT22, MQ2, LDR)
- [x] 1+ actionneur (on a 4: Buzzer, LED, Relais, LED statut)
- [x] Affichage (LCD I2C)
- [x] Alarmes visuelles et sonores
- [x] Configuration seuils

### Bonus
- [x] EEPROM pour persistance
- [x] Menu interactif
- [x] Support IoT (ThingSpeak)
- [x] Capteur additionnel (LDR)
- [x] Relais intelligent

---

## 🆘 Besoin d'Aide ?

### Problème: Code ne compile pas
**Solution:** Vérifier que `libraries.txt` est présent

### Problème: Capteurs ne répondent pas
**Solution:** Vérifier `diagram.json`

### Problème: LCD est noir
**Solution:** Attendre 2-3 secondes après le démarrage

---

## 📚 Documentation Complète

Pour plus de détails, consulter:
- 📖 **README.md** - Documentation principale
- 🎮 **GUIDE_WOKWI.md** - Guide de simulation
- 🧪 **SCENARIOS_TEST.md** - Tests détaillés
- 📄 **RAPPORT_EXEMPLE.md** - Template de rapport

---

## 🎓 Pour le Rapport

### Sections Principales
1. **Introduction** - Contexte et objectifs
2. **Conception** - Architecture et choix
3. **Réalisation** - Implémentation
4. **Tests** - Validation
5. **Extensions** - Améliorations
6. **Conclusion** - Bilan

### Captures d'Écran Recommandées
1. Vue d'ensemble du circuit
2. Serial Monitor (menu)
3. État normal sur LCD
4. État alarme (LED rouge)
5. Configuration via Serial
6. Dashboard ThingSpeak (optionnel)

---

## 💡 Conseils

1. **Tester un scénario à la fois**
2. **Utiliser le Serial Monitor** pour debugger
3. **Prendre des captures** pour le rapport
4. **Noter les problèmes** rencontrés
5. **Expliquer les solutions** trouvées

---

## 🌟 Fonctionnalités Impressionnantes

### À Montrer à l'Évaluation
1. **Alarmes multiples simultanées** → Déclencher plusieurs conditions
2. **Configuration via Serial** → Changer les seuils en direct
3. **Persistance EEPROM** → Redémarrer, les seuils sont sauvés
4. **Affichage rotatif** → LCD change automatiquement
5. **Relais intelligent** → S'active selon le contexte (ventilateur/extracteur)

---

## 🎯 Temps Estimés

- **Setup Wokwi:** 5 minutes
- **Premiers tests:** 10 minutes
- **Tests complets:** 30 minutes
- **Rédaction rapport:** 2-3 heures

**Total:** ~4 heures pour un projet complet

---

## 🚀 Go !

Vous avez tout ce qu'il faut pour réussir !

**Bon courage et bonne présentation ! 🎉**

---

**Questions ? Consultez:**
- README.md pour la doc complète
- SCENARIOS_TEST.md pour les tests
- GUIDE_WOKWI.md pour Wokwi

**Made with ❤️ for your success!**
