# 🏭 S.I.M.S. - Smart Industrial Monitoring System

## 📋 Description du Projet

Le **Smart Industrial Monitoring System (S.I.M.S.)** est un système de surveillance d'environnement industriel basé sur l'ESP32. Il surveille en temps réel plusieurs paramètres physiques et déclenche des alarmes lorsque des seuils critiques sont dépassés.

### 🎯 Objectifs
- Surveiller la température, l'humidité, la qualité de l'air (gaz) et la luminosité
- Afficher les données en temps réel sur un écran LCD
- Déclencher des alarmes visuelles et sonores en cas de dépassement de seuils
- Permettre la configuration des seuils via le terminal série
- Sauvegarder les configurations dans la mémoire EEPROM
- Support IoT avec ThingSpeak pour le monitoring à distance

---

## 🔧 Composants Utilisés

### Capteurs
| Capteur | Type | Pin | Description |
|---------|------|-----|-------------|
| **DHT22** | Numérique | GPIO15 | Mesure température et humidité |
| **MQ2** | Analogique | GPIO34 | Détection de gaz (GPL, CO, fumée) |
| **LDR** | Analogique | GPIO35 | Capteur de luminosité |

### Actionneurs
| Actionneur | Type | Pin | Description |
|------------|------|-----|-------------|
| **Buzzer** | PWM | GPIO25 | Alarme sonore |
| **LED Rouge** | Numérique | GPIO26 | Alarme visuelle |
| **Relais** | Numérique | GPIO27 | Contrôle ventilateur/extracteur |
| **LED Bleue** | Numérique | GPIO2 | Indicateur d'état système |

### Affichage
| Composant | Interface | Pins | Description |
|-----------|-----------|------|-------------|
| **LCD 16x2 I2C** | I2C | SDA:GPIO21, SCL:GPIO22 | Affichage des données |

---

## 📊 Fonctionnalités

### ✅ Fonctionnalités de Base (Requises)
- ✅ Utilisation de l'ESP32 dans Wokwi
- ✅ 3 capteurs intégrés (DHT22, MQ2, LDR)
- ✅ 3 actionneurs (Buzzer, LED, Relais)
- ✅ Affichage sur LCD I2C 16x2
- ✅ Alarmes visuelles et sonores
- ✅ Configuration des seuils via Serial Monitor

### 🌟 Fonctionnalités Avancées (Bonus)
- ✅ **Mémoire EEPROM**: Sauvegarde permanente des seuils configurés
- ✅ **Menu interactif**: Configuration complète via commandes série
- ✅ **Support IoT**: Envoi des données vers ThingSpeak
- ✅ **Affichage rotatif**: Rotation automatique des écrans LCD
- ✅ **LED d'état**: Indication visuelle du fonctionnement système
- ✅ **Relais intelligent**: Activation automatique en cas de danger

---

## 🎮 Utilisation

### Démarrage du Système

1. **Ouvrir le projet dans Wokwi**
   - Aller sur [wokwi.com](https://wokwi.com)
   - Créer un nouveau projet ESP32
   - Copier le contenu de `sketch.ino`, `diagram.json` et `libraries.txt`

2. **Lancer la simulation**
   - Cliquer sur le bouton vert "Start Simulation"
   - Ouvrir le Serial Monitor (icône en bas à droite)
   - Le système s'initialise et affiche le menu

### Commandes du Terminal Série

| Commande | Description | Exemple |
|----------|-------------|---------|
| `MENU` ou `M` | Afficher le menu principal | `MENU` |
| `STATUS` ou `S` | Afficher l'état actuel du système | `STATUS` |
| `SETTEMP <val>` | Définir le seuil de température max (°C) | `SETTEMP 35` |
| `SETHUM <val>` | Définir le seuil d'humidité max (%) | `SETHUM 80` |
| `SETGAS <val>` | Définir le seuil de gaz max (0-4095) | `SETGAS 2500` |
| `SETLIGHT <val>` | Définir le seuil de luminosité min (0-4095) | `SETLIGHT 300` |
| `RESET` | Réinitialiser aux valeurs par défaut | `RESET` |
| `HELP` ou `H` | Afficher l'aide | `HELP` |

### Seuils par Défaut

| Paramètre | Seuil par défaut | Unité |
|-----------|------------------|-------|
| Température max | 30.0 | °C |
| Humidité max | 70.0 | % |
| Gaz max | 2000 | 0-4095 |
| Luminosité min | 500 | 0-4095 |

---

## 🧪 Scénarios de Test

### Test 1: Température Élevée
1. Dans Wokwi, cliquer sur le capteur DHT22
2. Augmenter la température à 35°C
3. **Résultat attendu**:
   - Alarme sonore (buzzer)
   - LED rouge allumée
   - Relais activé (ventilateur)
   - Message "TEMP ELEVEE!" sur LCD

### Test 2: Détection de Gaz
1. Cliquer sur le joystick (simulant le MQ2)
2. Déplacer le joystick vers le haut (augmente la valeur analogique)
3. **Résultat attendu**:
   - Alarme activée
   - Message "GAZ DETECTE!"
   - Extracteur activé

### Test 3: Luminosité Faible
1. Cliquer sur le photorésisteur (LDR)
2. Réduire la luminosité en dessous du seuil
3. **Résultat attendu**:
   - Alarme activée
   - Message "LUMIERE FAIBLE!"

### Test 4: Configuration des Seuils
1. Dans le Serial Monitor, taper: `SETTEMP 25`
2. Augmenter la température du DHT22 à 26°C
3. **Résultat attendu**:
   - Alarme se déclenche car 26°C > 25°C (nouveau seuil)
   - Configuration sauvegardée en EEPROM

### Test 5: Conditions Normales
1. Régler tous les capteurs dans les plages normales
2. **Résultat attendu**:
   - Alarmes désactivées
   - LCD affiche "Systeme: OK"
   - LED bleue clignote (système actif)

---

## 📡 Configuration IoT (ThingSpeak)

### Étapes de Configuration

1. **Créer un compte ThingSpeak**
   - Aller sur [thingspeak.com](https://thingspeak.com)
   - Créer un nouveau channel

2. **Configurer les champs**
   - Field 1: Temperature
   - Field 2: Humidity
   - Field 3: Gas Level
   - Field 4: Light Level
   - Field 5: Alarm Status

3. **Obtenir la clé API**
   - Aller dans "API Keys"
   - Copier la "Write API Key"

4. **Modifier le code**
   ```cpp
   String thingSpeakApiKey = "VOTRE_CLE_API_ICI";
   ```

5. **Visualiser les données**
   - Les données sont envoyées toutes les 20 secondes
   - Créer des graphiques sur ThingSpeak Dashboard

---

## 🔌 Schéma de Câblage

### DHT22 (Température/Humidité)
```
DHT22 VCC  → ESP32 3V3
DHT22 GND  → ESP32 GND
DHT22 DATA → ESP32 GPIO15
```

### MQ2 (Capteur de Gaz - simulé par Joystick)
```
MQ2 VCC  → ESP32 3V3
MQ2 GND  → ESP32 GND
MQ2 A0   → ESP32 GPIO34 (ADC1)
```

### LDR (Photorésisteur)
```
LDR VCC → ESP32 3V3
LDR GND → ESP32 GND
LDR AO  → ESP32 GPIO35 (ADC1)
```

### LCD I2C 16x2
```
LCD VCC → ESP32 VIN (5V)
LCD GND → ESP32 GND
LCD SDA → ESP32 GPIO21
LCD SCL → ESP32 GPIO22
```

### Actionneurs
```
Buzzer (+) → ESP32 GPIO25
Buzzer (-) → ESP32 GND

LED Rouge (+) → Résistance 220Ω → ESP32 GPIO26
LED Rouge (-) → ESP32 GND

Relais VCC → ESP32 VIN (5V)
Relais GND → ESP32 GND
Relais IN  → ESP32 GPIO27

LED Bleue (+) → Résistance 220Ω → ESP32 GPIO2
LED Bleue (-) → ESP32 GND
```

---

## 📈 Architecture du Code

### Structure Principale
```
setup()
├── Initialisation des pins
├── Chargement EEPROM
├── Initialisation LCD
├── Initialisation DHT22
├── Connexion WiFi
└── Affichage menu

loop()
├── Lecture capteurs (toutes les 2s)
├── Vérification seuils
├── Mise à jour LCD (toutes les 500ms)
├── Envoi ThingSpeak (toutes les 20s)
├── Gestion commandes série
└── Rotation affichage (toutes les 5s)
```

### Fonctions Principales

| Fonction | Description |
|----------|-------------|
| `readSensors()` | Lit tous les capteurs |
| `checkThresholds()` | Vérifie les seuils et active alarmes |
| `updateDisplay()` | Met à jour l'affichage LCD |
| `activateAlarm()` | Active buzzer et LED |
| `deactivateAlarm()` | Désactive les alarmes |
| `saveThresholdsToEEPROM()` | Sauvegarde en mémoire |
| `loadThresholdsFromEEPROM()` | Charge depuis mémoire |
| `handleSerialCommands()` | Gère les commandes utilisateur |
| `sendToThingSpeak()` | Envoie données IoT |
| `showMenu()` | Affiche le menu interactif |

---

## 🚨 Logique des Alarmes

Le système déclenche des alarmes dans les cas suivants:

| Condition | Alarme | Actionneur |
|-----------|--------|------------|
| Température > Seuil | Sonore + Visuelle | Ventilateur ON |
| Humidité > Seuil | Sonore + Visuelle | - |
| Gaz > Seuil | Sonore + Visuelle | Extracteur ON |
| Luminosité < Seuil | Sonore + Visuelle | - |

**Note**: Le relais peut contrôler un ventilateur ou un extracteur selon le contexte industriel.

---

## 🎓 Points d'Évaluation Couverts

### Critères de Base ✅
- ✅ ESP32 dans Wokwi
- ✅ Au moins 2 capteurs (3 implémentés)
- ✅ Au moins 1 actionneur (4 implémentés)
- ✅ Affichage LCD
- ✅ Alarmes sonores et visuelles
- ✅ Configuration des seuils via Serial

### Extensions ✅
- ✅ Capteurs supplémentaires (LDR)
- ✅ Extension IoT (ThingSpeak)
- ✅ Mémoire EEPROM
- ✅ Menu de configuration interactif
- ✅ Code bien structuré et commenté

### Tests et Validation ✅
- ✅ Scénarios de test définis
- ✅ Validation sur Wokwi
- ✅ Documentation complète

---

## 📝 Rapport de Projet

### Informations Clés à Inclure

1. **Introduction**
   - Contexte industriel
   - Objectifs du système
   - Besoins identifiés

2. **Conception**
   - Architecture système
   - Choix des composants
   - Schéma de câblage

3. **Implémentation**
   - Structure du code
   - Algorithmes principaux
   - Gestion des alarmes

4. **Tests**
   - Scénarios testés
   - Résultats obtenus
   - Captures d'écran Wokwi

5. **Améliorations**
   - EEPROM pour persistance
   - IoT avec ThingSpeak
   - Menu interactif

6. **Conclusion**
   - Objectifs atteints
   - Difficultés rencontrées
   - Perspectives d'évolution

---

## 🔮 Évolutions Futures Possibles

1. **Capteurs supplémentaires**
   - Capteur de vibrations (ADXL345)
   - Capteur de pression (BMP280)
   - Capteur de particules (PM2.5)

2. **Connectivité avancée**
   - Application mobile Blynk
   - Dashboard web local
   - Notifications push

3. **Intelligence artificielle**
   - Prédiction de pannes
   - Apprentissage des patterns
   - Optimisation automatique des seuils

4. **Stockage de données**
   - Carte SD pour logs
   - Base de données locale
   - Historique des alarmes

---

## 👨‍💻 Auteur

Projet développé pour le cours de systèmes embarqués  
**Mini-Projet: Smart Industrial Monitoring System**

---

## 📄 Licence

Ce projet est à usage éducatif dans le cadre du mini-projet universitaire.

---

## 🆘 Support

Pour toute question ou problème:
1. Vérifier les connexions dans `diagram.json`
2. Consulter les messages du Serial Monitor
3. Tester les commandes une par une
4. Vérifier que les bibliothèques sont installées

---

## ✨ Remerciements

Merci d'utiliser ce système de surveillance industrielle intelligent!  
N'hésitez pas à le personnaliser selon vos besoins spécifiques.

**Bon courage pour votre présentation! 🎉**
