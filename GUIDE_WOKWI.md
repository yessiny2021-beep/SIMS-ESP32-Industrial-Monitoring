# Guide de Simulation sur Wokwi

## 🚀 Démarrage Rapide

### Étape 1: Créer un Nouveau Projet Wokwi
1. Aller sur [https://wokwi.com](https://wokwi.com)
2. Cliquer sur "New Project"
3. Sélectionner "ESP32"

### Étape 2: Importer les Fichiers

#### A. Code Principal (sketch.ino)
1. Supprimer le code par défaut dans l'éditeur
2. Copier tout le contenu du fichier `sketch.ino`
3. Coller dans l'éditeur Wokwi

#### B. Schéma de Circuit (diagram.json)
1. Cliquer sur l'icône "diagram.json" dans l'onglet des fichiers
2. Supprimer le contenu existant
3. Copier tout le contenu du fichier `diagram.json`
4. Coller dans l'éditeur

#### C. Bibliothèques (libraries.txt)
1. Cliquer sur le bouton "+" pour ajouter un fichier
2. Nommer le fichier "libraries.txt"
3. Copier le contenu du fichier `libraries.txt`
4. Coller et sauvegarder

### Étape 3: Lancer la Simulation
1. Cliquer sur le bouton vert "▶ Start Simulation" en haut
2. Attendre quelques secondes pour l'initialisation
3. Ouvrir le Serial Monitor (icône 💬 en bas)

---

## 🎮 Interaction avec le Système

### Dans le Serial Monitor
- Observer les messages d'initialisation
- Voir le menu de commandes s'afficher
- Taper des commandes pour configurer les seuils

### Exemples de Commandes
```
STATUS          → Voir l'état actuel
SETTEMP 35      → Température max = 35°C
SETHUM 80       → Humidité max = 80%
SETGAS 2500     → Gaz max = 2500
SETLIGHT 300    → Luminosité min = 300
```

### Sur l'Écran LCD
- L'affichage change automatiquement toutes les 5 secondes
- Affiche: Température/Humidité → Gaz/Luminosité → État système

---

## 🧪 Tester les Alarmes

### Test 1: Alarme Température
1. Cliquer sur le capteur DHT22 dans la simulation
2. Changer la température à 35°C
3. Cliquer "Update"
4. **Observer**: Buzzer sonne, LED rouge allumée, relais activé

### Test 2: Alarme Gaz
1. Cliquer sur le joystick (représente le capteur MQ2)
2. Déplacer le stick vers le haut au maximum
3. **Observer**: Alarme se déclenche, message sur LCD

### Test 3: Alarme Luminosité
1. Cliquer sur le photorésisteur (LDR)
2. Réduire la luminosité à 20%
3. **Observer**: Alarme pour luminosité faible

### Test 4: Retour à la Normale
1. Remettre tous les capteurs dans les valeurs normales
2. **Observer**: Alarmes s'arrêtent, LCD affiche "Systeme: OK"

---

## 📊 Indicateurs Visuels

| Composant | État Normal | État Alarme |
|-----------|-------------|-------------|
| LED Bleue | Clignote régulièrement | Continue de clignoter |
| LED Rouge | Éteinte | Allumée |
| Buzzer | Silencieux | Émet un son de 1000Hz |
| Relais | Ouvert (OFF) | Fermé (ON) |
| LCD | Affiche valeurs | Affiche "ALARME" |

---

## 🔧 Dépannage

### Le code ne compile pas
- Vérifier que `libraries.txt` est bien présent
- Vérifier qu'il n'y a pas d'erreur de copier-coller

### Les capteurs ne répondent pas
- Vérifier le fichier `diagram.json`
- S'assurer que toutes les connexions sont correctes

### WiFi ne se connecte pas
- C'est normal dans Wokwi, utiliser "Wokwi-GUEST" comme SSID
- Le système fonctionne sans WiFi pour les fonctions de base

### L'écran LCD est noir
- Attendre quelques secondes après le démarrage
- Vérifier que le backlight est activé dans le code

---

## 💡 Astuces

1. **Sauvegarder régulièrement**: Utilisez Ctrl+S pour sauvegarder
2. **Tester un scénario à la fois**: Plus facile à valider
3. **Observer le Serial Monitor**: Contient des informations détaillées
4. **Utiliser PAUSE**: Mettre en pause pour analyser l'état
5. **Screenshot**: Prendre des captures pour le rapport

---

## 📸 Captures Recommandées pour le Rapport

1. Vue d'ensemble du circuit Wokwi
2. Serial Monitor montrant le menu
3. LCD affichant les valeurs normales
4. État d'alarme (LED rouge + message)
5. Configuration d'un seuil via Serial
6. Graphique ThingSpeak (si configuré)

---

## ⚡ Raccourcis Wokwi

| Raccourci | Action |
|-----------|--------|
| `F1` | Démarrer/Arrêter simulation |
| `Ctrl + S` | Sauvegarder |
| `Ctrl + Shift + M` | Ouvrir Serial Monitor |
| `Échap` | Fermer dialog |

---

## 🎯 Checklist de Validation

Avant de présenter votre projet, vérifier:

- [ ] Le système démarre correctement
- [ ] Tous les capteurs sont fonctionnels
- [ ] Les alarmes se déclenchent aux bons seuils
- [ ] La configuration via Serial fonctionne
- [ ] L'EEPROM sauvegarde les seuils (tester en redémarrant)
- [ ] L'affichage LCD est clair et lisible
- [ ] Le buzzer émet un son en alarme
- [ ] Les LEDs s'allument correctement
- [ ] Le relais s'active en cas de danger

---

## 📹 Démonstration Recommandée

### Ordre de Présentation
1. **Démarrage**: Montrer l'initialisation
2. **Fonctionnement normal**: Valeurs dans les seuils
3. **Alarme température**: Déclencher et montrer réaction
4. **Alarme gaz**: Déclencher et montrer réaction
5. **Configuration**: Changer un seuil via Serial
6. **Retour normal**: Désactivation des alarmes
7. **Persistance**: Redémarrer pour montrer EEPROM

### Durée Estimée: 5-7 minutes

---

Bon succès avec votre simulation! 🎉
