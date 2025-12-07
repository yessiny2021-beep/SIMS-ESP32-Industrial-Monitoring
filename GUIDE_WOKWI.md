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

**Bibliothèques requises:**
- LiquidCrystal I2C (pour l'écran LCD)
- DHT sensor library (pour le capteur DHT22)
- PubSubClient (pour la communication MQTT)
- ArduinoJson (pour le formatage des données)

### Étape 3: Lancer la Simulation
1. Cliquer sur le bouton vert "▶ Start Simulation" en haut
2. Attendre quelques secondes pour l'initialisation
3. Ouvrir le Serial Monitor (icône 💬 en bas)
4. Observer les messages de connexion WiFi, MQTT et Web Server

---

## 🌐 Nouveautés: Interface Web et MQTT

### Fonctionnalités IoT Ajoutées
Le système S.I.M.S. intègre maintenant:
- **Communication MQTT** via broker.hivemq.com
- **Serveur Web** intégré sur l'ESP32
- **Interface Web moderne** avec graphiques temps réel
- **Contrôle à distance** des actionneurs
- **API REST** pour intégration externe

### Accéder à l'Interface Web

#### Option 1: Page Web Simple (intégrée)
1. Après le démarrage, noter l'adresse IP dans le Serial Monitor
2. Ouvrir un navigateur et aller à `http://<IP_ESP32>/`
3. Vous verrez une page avec les données en temps réel

#### Option 2: Interface Web Complète (data/index.html)
1. Télécharger le fichier `data/index.html` depuis le dépôt
2. Ouvrir ce fichier dans un navigateur moderne (Chrome, Firefox, Edge)
3. L'interface se connecte automatiquement au broker MQTT
4. Les données de l'ESP32 s'affichent en temps réel

### Tester la Communication MQTT

#### Depuis l'Interface Web:
1. Ouvrir `data/index.html` dans un navigateur
2. Vérifier le badge de statut "Connecté" (vert)
3. Observer les cartes de capteurs se mettre à jour toutes les 5 secondes
4. Les graphiques affichent l'historique des mesures

#### Contrôler le Relais via MQTT:
1. Dans l'interface web, cliquer sur le bouton "Relais: OFF"
2. Le bouton devient vert "Relais: ON"
3. Dans Wokwi, observer le module relais s'activer
4. Le Serial Monitor affiche "[MQTT] Relais activé manuellement"

#### Modifier les Seuils via Web:
1. Remplir le formulaire de configuration des seuils
2. Cliquer sur "Enregistrer les Seuils"
3. Les nouveaux seuils sont envoyés via MQTT
4. Vérifier dans le Serial Monitor: "[MQTT] Seuils mis à jour via MQTT"
5. Les valeurs sont sauvegardées dans l'EEPROM

### Tester l'API REST

#### Endpoint: /api/sensors
```bash
# Retourne les données des capteurs en JSON
http://<IP_ESP32>/api/sensors

# Exemple de réponse:
{
  "temperature": 25.5,
  "humidity": 60.0,
  "gas": 1500,
  "light": 2000,
  "alarm": false,
  "relay": false,
  "timestamp": 1234567890
}
```

#### Endpoint: /api/status
```bash
# Retourne le statut du système
http://<IP_ESP32>/api/status

# Exemple de réponse:
{
  "status": "online",
  "wifi": true,
  "mqtt": true,
  "alarm": false,
  "relay": false,
  "uptime": 3600,
  "ip": "192.168.1.100"
}
```

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
4. **Observer**: 
   - Buzzer sonne, LED rouge allumée, relais activé
   - Dans l'interface web, la carte température pulse en rouge
   - Les graphiques montrent le pic de température

### Test 2: Alarme Gaz
1. Cliquer sur le joystick (représente le capteur MQ2)
2. Déplacer le stick vers le haut au maximum
3. **Observer**: 
   - Alarme se déclenche, message sur LCD
   - Carte "Niveau Gaz" pulse en rouge dans l'interface web
   - Message dans la console de logs

### Test 3: Alarme Luminosité
1. Cliquer sur le photorésisteur (LDR)
2. Réduire la luminosité à 20%
3. **Observer**: 
   - Alarme pour luminosité faible
   - Interface web affiche l'alarme visuellement

### Test 4: Retour à la Normale
1. Remettre tous les capteurs dans les valeurs normales
2. **Observer**: 
   - Alarmes s'arrêtent, LCD affiche "Systeme: OK"
   - Les cartes dans l'interface web redeviennent normales

### Test 5: Contrôle à Distance (NOUVEAU)
1. Ouvrir l'interface web `data/index.html`
2. Cliquer sur "Relais: OFF" pour l'activer
3. **Observer**: 
   - Dans Wokwi, le relais s'active immédiatement
   - Le bouton devient vert "Relais: ON"
   - Serial Monitor: "[MQTT] Relais activé manuellement"

### Test 6: Configuration via Web (NOUVEAU)
1. Dans l'interface web, définir Température max = 25°C
2. Cliquer "Enregistrer les Seuils"
3. Dans Wokwi, augmenter la température à 26°C
4. **Observer**: 
   - L'alarme se déclenche immédiatement
   - Les nouveaux seuils sont sauvegardés en EEPROM

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
- S'assurer que les 4 bibliothèques sont listées (LiquidCrystal I2C, DHT sensor library, PubSubClient, ArduinoJson)

### Les capteurs ne répondent pas
- Vérifier le fichier `diagram.json`
- S'assurer que toutes les connexions sont correctes

### WiFi ne se connecte pas
- Dans Wokwi, utiliser "Wokwi-GUEST" comme SSID
- Pas de mot de passe nécessaire
- Attendre 10-20 secondes pour la connexion

### MQTT ne se connecte pas (NOUVEAU)
- Vérifier que WiFi est connecté d'abord
- broker.hivemq.com est un broker public, devrait fonctionner
- Observer le Serial Monitor pour les messages de connexion
- Attendre quelques secondes pour l'établissement de la connexion

### L'interface web ne reçoit pas de données (NOUVEAU)
- Vérifier que le fichier `data/index.html` est ouvert correctement
- Ouvrir la console du navigateur (F12) pour voir les erreurs
- S'assurer que le navigateur autorise les WebSocket
- Vérifier la connexion à broker.hivemq.com:8000
- Le badge de statut doit être vert "Connecté"

### L'écran LCD est noir
- Attendre quelques secondes après le démarrage
- Vérifier que le backlight est activé dans le code

### Les graphiques ne s'affichent pas (NOUVEAU)
- Vérifier la connexion internet (Chart.js chargé depuis CDN)
- Attendre quelques secondes pour l'accumulation de données
- Rafraîchir la page si nécessaire

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
2. Serial Monitor montrant le menu et connexions (WiFi, MQTT, Web Server)
3. LCD affichant les valeurs normales
4. État d'alarme (LED rouge + message)
5. Configuration d'un seuil via Serial
6. **NOUVEAU**: Interface web avec données temps réel
7. **NOUVEAU**: Graphiques montrant l'historique des mesures
8. **NOUVEAU**: Contrôle du relais via l'interface web
9. **NOUVEAU**: Console de logs dans l'interface web
10. Graphique ThingSpeak (si configuré)

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

### Fonctionnalités de Base
- [ ] Le système démarre correctement
- [ ] Tous les capteurs sont fonctionnels
- [ ] Les alarmes se déclenchent aux bons seuils
- [ ] La configuration via Serial fonctionne
- [ ] L'EEPROM sauvegarde les seuils (tester en redémarrant)
- [ ] L'affichage LCD est clair et lisible
- [ ] Le buzzer émet un son en alarme
- [ ] Les LEDs s'allument correctement
- [ ] Le relais s'active en cas de danger

### Fonctionnalités IoT (NOUVELLES)
- [ ] WiFi se connecte automatiquement
- [ ] MQTT se connecte au broker
- [ ] Serveur Web démarre correctement
- [ ] Page web simple accessible via IP
- [ ] Interface web complète se connecte au MQTT
- [ ] Données capteurs affichées en temps réel
- [ ] Graphiques se mettent à jour automatiquement
- [ ] Contrôle du relais via web fonctionnel
- [ ] Modification des seuils via web opérationnelle
- [ ] Console de logs affiche les événements
- [ ] API REST retourne les bonnes données

---

## 📹 Démonstration Recommandée

### Ordre de Présentation
1. **Démarrage**: Montrer l'initialisation (WiFi, MQTT, Web Server)
2. **Fonctionnement normal**: Valeurs dans les seuils
3. **Interface Web**: Présenter le dashboard et les graphiques
4. **Alarme température**: Déclencher et montrer réaction (Wokwi + Web)
5. **Alarme gaz**: Déclencher et montrer réaction
6. **Contrôle à distance**: Activer le relais via l'interface web
7. **Configuration**: Changer un seuil via l'interface web
8. **API REST**: Montrer les endpoints JSON
9. **Retour normal**: Désactivation des alarmes
10. **Persistance**: Redémarrer pour montrer EEPROM

### Durée Estimée: 8-10 minutes

### Points Forts à Mettre en Avant
- ✅ Architecture complète IoT (capteurs → ESP32 → MQTT → Web)
- ✅ Interface moderne et professionnelle
- ✅ Contrôle bidirectionnel (lecture + commande)
- ✅ Visualisation temps réel avec graphiques
- ✅ API REST pour intégration externe
- ✅ Persistance des données (EEPROM)
- ✅ Compatible Wokwi pour simulation et tests

---

Bon succès avec votre simulation! 🎉

## 🌐 Ressources Complémentaires

### Documentation Technique
- **README.md**: Documentation complète du projet
- **SCENARIOS_TEST.md**: Scénarios de test détaillés
- **GUIDE_PRESENTATION.md**: Guide pour la présentation

### Liens Utiles
- [Wokwi](https://wokwi.com): Simulateur en ligne
- [HiveMQ](https://www.hivemq.com): Broker MQTT public
- [Chart.js](https://www.chartjs.org): Documentation des graphiques
- [Bootstrap](https://getbootstrap.com): Framework CSS

### Support
Pour toute question ou problème:
1. Consulter le README.md
2. Vérifier les messages du Serial Monitor
3. Inspecter la console du navigateur (F12)
4. Tester les endpoints API individuellement
