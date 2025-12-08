# Changelog - Interface Web et MQTT v2.0

## 🎯 Vue d'Ensemble

Cette mise à jour majeure ajoute une interface web complète avec communication MQTT bidirectionnelle au système S.I.M.S. (Smart Industrial Monitoring System), transformant le projet en une solution IoT moderne et professionnelle.

## 📦 Fichiers Modifiés/Créés

### Nouveaux Fichiers
1. **`data/index.html`** (722 lignes)
   - Interface web responsive avec Bootstrap 5
   - Graphiques temps réel avec Chart.js
   - Communication MQTT via WebSocket
   - Console de logs et contrôles interactifs

### Fichiers Modifiés
1. **`sketch.ino`** (914 lignes, +402 lignes)
   - Intégration MQTT avec PubSubClient
   - Serveur Web HTTP intégré
   - API REST (/api/sensors, /api/status)
   - Gestion des commandes MQTT
   - Validation robuste des entrées

2. **`libraries.txt`** (14 lignes, +4 lignes)
   - Ajout de PubSubClient (MQTT)
   - Ajout de ArduinoJson (formatage JSON)

3. **`README.md`** (683 lignes, +308 lignes)
   - Documentation complète de l'architecture MQTT
   - Guide d'utilisation de l'interface web
   - Documentation API REST
   - Guide de déploiement détaillé
   - Nouveaux scénarios de test
   - Section dépannage étendue

4. **`GUIDE_WOKWI.md`** (362 lignes, +201 lignes)
   - Instructions pour tester MQTT dans Wokwi
   - Guide d'utilisation de l'interface web
   - Nouveaux scénarios de test IoT
   - Checklist de validation étendue

## ✨ Nouvelles Fonctionnalités

### 🌐 Communication MQTT

#### Broker MQTT
- **Serveur**: broker.hivemq.com (broker public gratuit)
- **Port ESP32**: 1883 (MQTT standard)
- **Port WebSocket**: 8000 (pour navigateur web)
- **Authentification**: Aucune (accès public pour tests)
- **Client ID unique**: Généré à partir de l'adresse MAC de l'ESP32

#### Topics Implémentés

**Publication (ESP32 → Web):**
- `sims/sensors/data` - Données de tous les capteurs (JSON, toutes les 5s)
- `sims/status` - Statut du système (JSON)

**Souscription (Web → ESP32):**
- `sims/control/relay` - Contrôle du relais (ON/OFF)
- `sims/control/alarm` - Contrôle manuel de l'alarme (ON/OFF)
- `sims/control/thresholds` - Modification des seuils
- `sims/control/refresh` - Force la publication des données

#### Format des Messages

**Données capteurs:**
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

**Commande relais:**
```json
{"state": "ON"}  // ou "OFF"
```

**Modification seuils:**
```json
{
  "temp": 35,
  "hum": 75,
  "gas": 2500,
  "light": 600
}
```

### 🌍 Serveur Web Intégré

#### Routes HTTP
- **`/`** - Page d'accueil avec informations système (HTML)
- **`/api/sensors`** - Données capteurs en temps réel (JSON)
- **`/api/status`** - Statut complet du système (JSON)

#### Caractéristiques
- Serveur HTTP sur port 80
- Démarrage automatique au boot
- Compatible avec Wokwi et production
- Réponses JSON pour intégration facile

### 🖥️ Interface Web Moderne

#### Composants Visuels
1. **En-tête Dynamique**
   - Badge de statut MQTT (connecté/déconnecté)
   - Heure de dernière mise à jour
   - Logo S.I.M.S.

2. **Cartes de Capteurs** (4 cartes)
   - Température avec icône thermomètre
   - Humidité avec icône goutte d'eau
   - Gaz avec icône nuage
   - Luminosité avec icône ampoule
   - Animation de pulsation en cas d'alarme
   - Affichage des seuils configurés

3. **Graphiques Temps Réel** (2 graphiques)
   - Température & Humidité sur un même graphe
   - Gaz & Luminosité avec deux axes Y
   - Historique de 20 points maximum
   - Mise à jour automatique toutes les 5s
   - Interaction au survol

4. **Panneau de Contrôles**
   - Bouton ON/OFF pour le relais
   - Bouton ON/OFF pour l'alarme manuelle
   - Bouton de rafraîchissement
   - Indicateurs visuels d'état (vert/gris)

5. **Formulaire de Configuration**
   - Champs pour température max
   - Champs pour humidité max
   - Champs pour niveau gaz max
   - Champs pour luminosité min
   - Validation côté client
   - Bouton d'enregistrement

6. **Console de Logs**
   - Affichage en temps réel
   - 3 types de messages (info, warning, error)
   - Défilement automatique
   - Horodatage de chaque événement
   - Limitation à 100 logs

#### Technologies Utilisées
- **Bootstrap 5.3.0** - Framework CSS responsive
- **Chart.js 4.3.0** - Graphiques interactifs
- **MQTT.js 5.0.0** - Client MQTT pour navigateur
- **Font Awesome 6.4.0** - Icônes vectorielles

## 🔒 Améliorations de Sécurité

### Validations Implémentées

1. **ID Client MQTT Unique**
   - ESP32: Basé sur l'adresse MAC WiFi
   - Web: Timestamp + crypto.getRandomValues()
   - Évite les conflits de connexion

2. **Validation des Entrées MQTT**
   - Vérification NULL avant strcmp
   - Plages de valeurs pour les seuils:
     - Température: 0-100°C
     - Humidité: 0-100%
     - Gaz: 0-4095
     - Luminosité: 0-4095
   - Messages d'erreur descriptifs

3. **Gestion d'Erreurs Robuste**
   - Messages d'erreur MQTT descriptifs (10 codes)
   - Validation JSON avant traitement
   - Buffer JSON augmenté (256 bytes)
   - Reconnexion automatique MQTT

4. **Pratiques Sécurisées**
   - Pas de secrets hardcodés
   - Broker MQTT public pour tests uniquement
   - Validation côté serveur et client
   - Utilisation de crypto API moderne

## 📊 Modifications du Code

### Nouveaux Includes
```cpp
#include <PubSubClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
```

### Nouvelles Variables Globales
```cpp
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WebServer webServer(80);
bool manualRelayControl = false;
bool manualAlarmControl = false;
unsigned long lastMqttPublish = 0;
```

### Nouvelles Fonctions
```cpp
void connectMQTT();                    // Connexion au broker MQTT
void mqttCallback(...);                 // Traitement messages MQTT
void publishSensorData();               // Publication données
void setupWebServer();                  // Configuration serveur web
void handleRoot();                      // Route /
void handleStatus();                    // Route /api/status
void handleSensors();                   // Route /api/sensors
void handleNotFound();                  // 404 handler
```

### Loop() Modifié
```cpp
void loop() {
  // Maintenir connexion MQTT
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();
  
  // Gérer requêtes Web
  webServer.handleClient();
  
  // Publication MQTT (5s)
  if (currentTime - lastMqttPublish >= 5000) {
    publishSensorData();
  }
  
  // [reste du code existant préservé]
}
```

## 🧪 Nouveaux Scénarios de Test

### Test 6: Communication MQTT
- Ouverture de l'interface web
- Vérification de la connexion
- Réception des données en temps réel
- Mise à jour automatique des graphiques

### Test 7: Contrôle à Distance
- Activation du relais via l'interface web
- Vérification dans Wokwi
- Confirmation dans Serial Monitor

### Test 8: Modification des Seuils
- Changement via formulaire web
- Validation dans EEPROM
- Test de déclenchement d'alarme

### Test 9: API REST
- Test endpoint /api/sensors
- Test endpoint /api/status
- Vérification format JSON

### Test 10: Reconnexion MQTT
- Simulation de déconnexion
- Vérification reconnexion automatique
- Republication des données

## 📈 Métriques du Projet

### Lignes de Code
- **Avant**: ~512 lignes
- **Après**: ~914 lignes (sketch.ino)
- **Ajouté**: 722 lignes (interface web)
- **Total projet**: ~2695 lignes

### Fonctionnalités
- **Capteurs**: 3 (DHT22, MQ2, LDR)
- **Actionneurs**: 4 (Buzzer, LED rouge, Relais, LED bleue)
- **Topics MQTT**: 6 (2 pub, 4 sub)
- **Routes HTTP**: 3 (/, /api/sensors, /api/status)
- **Graphiques**: 2 (Temp/Hum, Gaz/Lumière)

### Documentation
- **README.md**: 683 lignes (+308)
- **GUIDE_WOKWI.md**: 362 lignes (+201)
- **Scénarios de test**: 10 (+6)

## 🎓 Compétences Démontrées

### Techniques
1. **IoT & Communication**
   - Protocole MQTT
   - WebSocket
   - API REST
   - JSON

2. **Développement Web**
   - HTML5/CSS3/JavaScript
   - Bootstrap 5 (Responsive Design)
   - Chart.js (Data Visualization)
   - AJAX/Fetch API

3. **Embedded Systems**
   - ESP32 WiFi
   - Gestion multi-tâches
   - Optimisation mémoire
   - Gestion d'état

4. **Architecture**
   - Client-Serveur
   - Publish-Subscribe (MQTT)
   - API REST
   - Séparation des préoccupations

5. **Sécurité**
   - Validation des entrées
   - Gestion d'erreurs
   - ID uniques
   - Pratiques sécurisées

## 🚀 Utilisation

### Déploiement Rapide

1. **Dans Wokwi:**
   ```
   1. Copier sketch.ino, diagram.json, libraries.txt
   2. Lancer la simulation
   3. Noter l'IP dans Serial Monitor
   ```

2. **Interface Web:**
   ```
   1. Ouvrir data/index.html dans un navigateur
   2. Connexion automatique au MQTT
   3. Les données s'affichent en temps réel
   ```

3. **Test API:**
   ```bash
   curl http://<IP_ESP32>/api/sensors
   curl http://<IP_ESP32>/api/status
   ```

## 📝 Compatibilité

### Testé avec:
- ✅ Wokwi Simulator
- ✅ Chrome 120+
- ✅ Firefox 121+
- ✅ Edge 120+

### Bibliothèques Requises:
- LiquidCrystal I2C
- DHT sensor library
- PubSubClient (MQTT)
- ArduinoJson

### Dépendances CDN:
- Bootstrap 5.3.0
- Chart.js 4.3.0
- MQTT.js 5.0.0
- Font Awesome 6.4.0

## 🏆 Points Forts

1. **Architecture Professionnelle**
   - Séparation client/serveur
   - Communication standardisée (MQTT/REST)
   - Code modulaire et maintenable

2. **Interface Moderne**
   - Design responsive
   - Graphiques interactifs
   - Expérience utilisateur intuitive

3. **Robustesse**
   - Gestion d'erreurs complète
   - Validation des entrées
   - Reconnexion automatique

4. **Documentation Complète**
   - README détaillé
   - Guide Wokwi complet
   - Exemples d'utilisation

5. **Évolutivité**
   - Architecture extensible
   - API REST pour intégrations
   - Support multi-clients MQTT

## 🔮 Évolutions Futures Possibles

1. **Authentification**
   - Login/password MQTT
   - JWT pour API REST
   - Gestion des utilisateurs

2. **Stockage**
   - Base de données historique
   - Export CSV/JSON
   - Logs persistants

3. **Notifications**
   - Email alerts
   - Push notifications
   - Webhooks

4. **Mobile**
   - Application mobile native
   - Progressive Web App (PWA)
   - Notifications push mobile

5. **Analytics**
   - Tableaux de bord avancés
   - Rapports automatiques
   - Prédictions ML

## 📞 Support

Pour toute question ou problème:
1. Consulter README.md et GUIDE_WOKWI.md
2. Vérifier la console du navigateur (F12)
3. Observer les messages Serial Monitor
4. Tester les endpoints API individuellement

## 👏 Remerciements

Merci d'utiliser S.I.M.S. v2.0 avec interface web et MQTT!

Cette mise à jour transforme le projet en une solution IoT complète et professionnelle, démontrant une maîtrise des technologies modernes de l'Internet des Objets.

**Bon courage pour votre présentation! 🎉**

---

**Version**: 2.0  
**Date**: Décembre 2024  
**Auteur**: Projet S.I.M.S.  
**Licence**: Usage Éducatif
