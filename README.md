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

### 🌐 Nouvelles Fonctionnalités IoT
- ✅ **Communication MQTT**: Protocole léger pour IoT avec broker public (broker.hivemq.com)
- ✅ **Interface Web Moderne**: Dashboard responsive avec Bootstrap 5
- ✅ **Graphiques Temps Réel**: Visualisation de l'historique avec Chart.js
- ✅ **Contrôle à Distance**: Commandes via MQTT (relais, alarme, seuils)
- ✅ **API REST**: Endpoints JSON pour intégration externe
- ✅ **Console de Logs**: Suivi des événements en temps réel
- ✅ **Compatible Wokwi**: Fonctionnement complet dans le simulateur

---

## 🌐 Architecture MQTT et Interface Web

### Broker MQTT
Le système utilise le broker MQTT public **broker.hivemq.com** qui est gratuit et compatible avec Wokwi:
- **Serveur**: broker.hivemq.com
- **Port ESP32**: 1883 (MQTT standard)
- **Port WebSocket**: 8000 (pour navigateur web)
- **Authentification**: Aucune (accès public)

### Topics MQTT Implémentés

#### 📤 Topics de Publication (ESP32 → Web)
| Topic | Description | Format | Fréquence |
|-------|-------------|--------|-----------|
| `sims/sensors/data` | Données de tous les capteurs | JSON | 5 secondes |
| `sims/status` | Statut du système | JSON | À la connexion |

**Exemple de données capteurs:**
```json
{
  "temperature": 25.5,
  "humidity": 60.0,
  "gas": 1500,
  "light": 2000,
  "alarm": false,
  "relay": false,
  "wifi": true,
  "timestamp": 1234567890
}
```

#### 📥 Topics de Souscription (Web → ESP32)
| Topic | Description | Format | Action |
|-------|-------------|--------|--------|
| `sims/control/relay` | Contrôle du relais | `{"state": "ON/OFF"}` | Active/Désactive le relais |
| `sims/control/alarm` | Contrôle de l'alarme | `{"state": "ON/OFF"}` | Active/Désactive l'alarme manuellement |
| `sims/control/thresholds` | Modification des seuils | `{"temp": 35, "hum": 75, "gas": 2500, "light": 600}` | Met à jour les seuils |
| `sims/control/refresh` | Demande de rafraîchissement | `1` | Force la publication des données |

### Serveur Web Intégré

L'ESP32 héberge un serveur web sur le port 80 avec les routes suivantes:

| Route | Méthode | Description | Retour |
|-------|---------|-------------|--------|
| `/` | GET | Page d'accueil avec infos système | HTML |
| `/api/status` | GET | Statut du système | JSON |
| `/api/sensors` | GET | Données des capteurs | JSON |

**Exemple d'accès:**
```
http://<IP_ESP32>/
http://<IP_ESP32>/api/sensors
http://<IP_ESP32>/api/status
```

### Interface Web Complète

L'interface web (`data/index.html`) offre:

1. **Dashboard Temps Réel**
   - Cartes affichant température, humidité, gaz, luminosité
   - Indicateurs visuels d'alarme (animation pulsante)
   - Affichage des seuils configurés

2. **Graphiques Interactifs**
   - Historique température & humidité
   - Historique gaz & luminosité
   - Jusqu'à 20 points de données

3. **Contrôles à Distance**
   - Boutons ON/OFF pour relais
   - Boutons ON/OFF pour alarme
   - Bouton de rafraîchissement

4. **Configuration des Seuils**
   - Formulaire pour température, humidité, gaz, luminosité
   - Validation en temps réel
   - Sauvegarde automatique dans EEPROM

5. **Console de Logs**
   - Événements système
   - Connexion/déconnexion MQTT
   - Commandes envoyées/reçues
   - Messages d'erreur

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

3. **Accéder à l'interface web**
   - Le Serial Monitor affiche l'adresse IP de l'ESP32
   - Ouvrir un navigateur et accéder à `http://<IP_ESP32>/`
   - Vous verrez la page d'accueil du serveur web

4. **Utiliser l'interface web complète**
   - Ouvrir le fichier `data/index.html` dans votre navigateur
   - L'interface se connecte automatiquement au broker MQTT
   - Les données s'affichent en temps réel dès la connexion

### Utilisation de l'Interface Web

#### Connexion MQTT
1. L'interface se connecte automatiquement à `broker.hivemq.com:8000`
2. Le badge de statut passe au vert quand la connexion est établie
3. Les données des capteurs s'affichent toutes les 5 secondes

#### Contrôle du Relais
1. Cliquer sur le bouton "Relais: OFF"
2. Le bouton passe en vert et affiche "Relais: ON"
3. Sur l'ESP32, le relais s'active immédiatement
4. Mode manuel activé - le relais ne se désactivera pas automatiquement

#### Contrôle de l'Alarme
1. Cliquer sur le bouton "Alarme: OFF"
2. Le buzzer et la LED rouge s'activent
3. Mode manuel - l'alarme reste active même si les conditions sont normales
4. Cliquer à nouveau pour désactiver

#### Modification des Seuils
1. Remplir le formulaire de configuration avec les nouvelles valeurs
2. Cliquer sur "Enregistrer les Seuils"
3. Les seuils sont envoyés via MQTT à l'ESP32
4. Les valeurs sont sauvegardées dans l'EEPROM
5. Les indicateurs se mettent à jour avec les nouveaux seuils

#### Visualisation des Graphiques
- Les graphiques se mettent à jour automatiquement
- Conservent jusqu'à 20 points de données
- Deux axes Y pour le graphique Gaz/Luminosité
- Survol pour voir les valeurs exactes

#### Console de Logs
- Affiche tous les événements système
- Messages MQTT reçus/envoyés
- Erreurs et avertissements
- Défilement automatique vers le bas


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

### Test 6: Communication MQTT
1. Ouvrir `data/index.html` dans un navigateur
2. Vérifier que le badge de statut est vert (Connecté)
3. Modifier un capteur dans Wokwi
4. **Résultat attendu**:
   - Les valeurs se mettent à jour dans l'interface web
   - Les graphiques affichent les nouvelles données
   - Les cartes de capteurs montrent les valeurs en temps réel

### Test 7: Contrôle à Distance via MQTT
1. Dans l'interface web, cliquer sur "Relais: OFF"
2. **Résultat attendu**:
   - Le bouton devient vert "Relais: ON"
   - Dans Wokwi, le relais s'active (LED du module relais)
   - Le Serial Monitor affiche "[MQTT] Relais activé manuellement"

### Test 8: Modification des Seuils via Web
1. Dans l'interface web, modifier la température max à 25°C
2. Cliquer sur "Enregistrer les Seuils"
3. Dans Wokwi, augmenter la température à 26°C
4. **Résultat attendu**:
   - L'alarme se déclenche dans Wokwi
   - La carte température dans l'interface web pulse en rouge
   - Console de logs affiche "ALARME ACTIVE!"

### Test 9: API REST
1. Ouvrir un navigateur à `http://<IP_ESP32>/api/sensors`
2. **Résultat attendu**:
   - Retourne un JSON avec toutes les données capteurs
   - Format: `{"temperature": 25.5, "humidity": 60.0, ...}`
3. Accéder à `http://<IP_ESP32>/api/status`
4. **Résultat attendu**:
   - Retourne le statut du système en JSON
   - Inclut WiFi, MQTT, uptime, IP

### Test 10: Reconnexion Automatique MQTT
1. Simuler une déconnexion MQTT (fermer/rouvrir l'interface web)
2. **Résultat attendu**:
   - L'ESP32 détecte la déconnexion
   - Reconnexion automatique dans les 5 secondes
   - Republication des données capteurs

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
- ✅ Extension IoT (ThingSpeak + MQTT)
- ✅ Mémoire EEPROM
- ✅ Menu de configuration interactif
- ✅ Code bien structuré et commenté
- ✅ **Interface Web Moderne**
- ✅ **Communication MQTT Bidirectionnelle**
- ✅ **API REST**
- ✅ **Contrôle à Distance**

### Tests et Validation ✅
- ✅ Scénarios de test définis
- ✅ Validation sur Wokwi
- ✅ Documentation complète

---

## 🚀 Guide de Déploiement

### Déploiement sur Wokwi

#### Étape 1: Préparation des Fichiers
1. Créer un nouveau projet ESP32 sur [wokwi.com](https://wokwi.com)
2. Copier le contenu de `sketch.ino` dans l'éditeur principal
3. Copier le contenu de `diagram.json` (onglet diagram.json)
4. Copier le contenu de `libraries.txt` (créer le fichier si nécessaire)

#### Étape 2: Configuration
1. Vérifier que le WiFi est configuré sur "Wokwi-GUEST"
2. Pas de modification nécessaire pour le broker MQTT (déjà configuré)
3. Sauvegarder le projet

#### Étape 3: Lancement
1. Cliquer sur "Start Simulation" (bouton vert)
2. Observer le Serial Monitor pour:
   - Connexion WiFi réussie
   - Connexion MQTT établie
   - Serveur Web démarré
   - Adresse IP attribuée

#### Étape 4: Test de l'Interface Web
1. Noter l'adresse IP affichée dans le Serial Monitor
2. Ouvrir un navigateur et accéder à `http://<IP>/`
3. Vérifier que la page d'accueil s'affiche
4. Tester les routes API: `/api/sensors` et `/api/status`

#### Étape 5: Interface Web Complète
1. Télécharger le fichier `data/index.html` depuis ce dépôt
2. Ouvrir le fichier dans un navigateur web moderne (Chrome, Firefox, Edge)
3. L'interface se connecte automatiquement au broker MQTT
4. Les données s'affichent en temps réel

### Configuration pour Production

#### Si vous utilisez un vrai ESP32:

1. **WiFi Personnel**
   ```cpp
   const char* ssid = "VotreSSID";
   const char* password = "VotreMotDePasse";
   ```

2. **Broker MQTT Privé** (optionnel)
   ```cpp
   const char* mqttServer = "votre-broker.com";
   const int mqttPort = 1883;
   const char* mqttUser = "username";      // Ajouter
   const char* mqttPassword = "password";  // Ajouter
   ```

3. **Upload du Système de Fichiers**
   - Utiliser l'outil "ESP32 Sketch Data Upload" dans Arduino IDE
   - Placer `index.html` dans le dossier `data/`
   - L'ESP32 servira le fichier depuis SPIFFS

### Dépannage

#### Problème: WiFi ne se connecte pas
- **Solution**: Vérifier le SSID et mot de passe
- Dans Wokwi, utiliser uniquement "Wokwi-GUEST" sans mot de passe

#### Problème: MQTT ne se connecte pas
- **Solution**: 
  - Vérifier que le broker est accessible
  - broker.hivemq.com est public et devrait fonctionner
  - Augmenter le délai de reconnexion si nécessaire

#### Problème: Interface web ne reçoit pas de données
- **Solution**:
  - Vérifier la connexion WebSocket (broker.hivemq.com:8000)
  - Ouvrir la console développeur du navigateur (F12)
  - Vérifier les erreurs MQTT
  - S'assurer que le navigateur autorise les connexions WebSocket

#### Problème: API REST inaccessible
- **Solution**:
  - Vérifier l'adresse IP de l'ESP32 dans le Serial Monitor
  - S'assurer que le serveur web a démarré (message "[OK] Serveur Web démarré")
  - Tester avec curl: `curl http://<IP>/api/sensors`

#### Problème: Graphiques ne s'affichent pas
- **Solution**:
  - Vérifier que Chart.js est chargé (connexion internet requise)
  - Ouvrir la console du navigateur pour voir les erreurs
  - Attendre quelques secondes pour l'accumulation de données

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
   - IoT avec ThingSpeak et MQTT
   - Menu interactif
   - Interface web moderne
   - Communication bidirectionnelle
   - Contrôle à distance

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
   - Capteur de courant (INA219)

2. **Connectivité avancée**
   - ✅ ~~Dashboard web local~~ (Implémenté!)
   - ✅ ~~Communication MQTT~~ (Implémenté!)
   - Application mobile native (React Native)
   - Notifications push (Firebase Cloud Messaging)
   - Support MQTT sécurisé (TLS/SSL)

3. **Intelligence artificielle**
   - Prédiction de pannes avec ML
   - Apprentissage des patterns normaux
   - Optimisation automatique des seuils
   - Détection d'anomalies

4. **Stockage de données**
   - Carte SD pour logs historiques
   - Base de données locale (SQLite)
   - Export CSV/JSON
   - Historique des alarmes avec timestamps

5. **Améliorations Interface Web**
   - Mode sombre/clair
   - Tableaux de bord personnalisables
   - Alertes email automatiques
   - Export de rapports PDF
   - Multi-langues (FR/EN)

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
