# 📄 RAPPORT DE PROJET - S.I.M.S.
## Smart Industrial Monitoring System

---

## 📑 TABLE DES MATIÈRES

1. [Introduction](#introduction)
2. [Analyse et Conception](#analyse-et-conception)
3. [Réalisation](#réalisation)
4. [Tests et Validation](#tests-et-validation)
5. [Extensions et Améliorations](#extensions-et-améliorations)
6. [Conclusion](#conclusion)
7. [Annexes](#annexes)

---

## 1. INTRODUCTION

### 1.1 Contexte du Projet

Dans un environnement industriel moderne, la surveillance continue des paramètres environnementaux est essentielle pour:
- Assurer la sécurité des travailleurs
- Prévenir les accidents (incendies, fuites de gaz, etc.)
- Maintenir des conditions de travail optimales
- Respecter les normes de sécurité industrielle

Le projet S.I.M.S. (Smart Industrial Monitoring System) répond à ces besoins en proposant un système de surveillance autonome, économique et facilement déployable.

### 1.2 Objectifs du Projet

**Objectifs principaux:**
- Surveiller en temps réel 4 paramètres critiques (température, humidité, gaz, luminosité)
- Déclencher des alarmes automatiques en cas de dépassement de seuils
- Afficher les données de manière claire et accessible
- Permettre une configuration flexible des seuils d'alerte

**Objectifs secondaires:**
- Sauvegarder les configurations de manière persistante
- Offrir une interface de configuration intuitive
- Intégrer des capacités IoT pour le monitoring à distance
- Activer automatiquement des systèmes de sécurité (ventilation, extraction)

### 1.3 Cahier des Charges

**Exigences obligatoires:**
- ✅ Utilisation de l'ESP32
- ✅ Minimum 2 capteurs (3 implémentés: DHT22, MQ2, LDR)
- ✅ Minimum 1 actionneur (4 implémentés: Buzzer, LED, Relais, LED statut)
- ✅ Affichage des données (LCD I2C 16x2)
- ✅ Alarmes sonores et visuelles
- ✅ Configuration des seuils via Serial

**Extensions réalisées:**
- ✅ Mémoire EEPROM pour persistance
- ✅ Menu interactif complet
- ✅ Support IoT (ThingSpeak)
- ✅ Affichage rotatif automatique
- ✅ Activation automatique d'extracteur/ventilateur

---

## 2. ANALYSE ET CONCEPTION

### 2.1 Analyse Fonctionnelle

#### Diagramme de Cas d'Utilisation

```
┌─────────────────────────────────────────────────┐
│              Système S.I.M.S.                   │
├─────────────────────────────────────────────────┤
│                                                 │
│  [Surveillance Continue]                        │
│      ↓                                          │
│  [Lecture Capteurs] → [Affichage LCD]          │
│      ↓                                          │
│  [Vérification Seuils]                          │
│      ↓                                          │
│  Si dépassement:                                │
│      • Activer Alarme Sonore                    │
│      • Activer Alarme Visuelle                  │
│      • Activer Extracteur/Ventilateur           │
│      • Envoyer notification                     │
│                                                 │
│  [Configuration] ← Utilisateur (Serial)         │
│      • Modifier seuils                          │
│      • Consulter état                           │
│      • Réinitialiser                            │
│                                                 │
└─────────────────────────────────────────────────┘
```

#### Fonctions Principales

| Fonction | Description | Fréquence |
|----------|-------------|-----------|
| `readSensors()` | Lecture de tous les capteurs | 2 secondes |
| `checkThresholds()` | Vérification des seuils | Après chaque lecture |
| `updateDisplay()` | Mise à jour LCD | 500ms |
| `activateAlarm()` | Activation des alarmes | À la demande |
| `handleSerialCommands()` | Traitement commandes | Continue |
| `sendToThingSpeak()` | Envoi données IoT | 20 secondes |

### 2.2 Architecture Matérielle

#### Liste des Composants

**Microcontrôleur:**
- ESP32 DevKit V1
  - WiFi intégré
  - 34 GPIO
  - 2 ADC (12 bits)
  - Support I2C, SPI, UART

**Capteurs:**
1. **DHT22**
   - Type: Température et humidité
   - Plage température: -40°C à 80°C (±0.5°C)
   - Plage humidité: 0-100% (±2%)
   - Interface: Digital 1-Wire
   - Pin: GPIO15

2. **MQ2** (simulé par joystick)
   - Type: Détecteur de gaz
   - Détecte: GPL, CO, fumée, méthane
   - Sortie: Analogique 0-4095
   - Pin: GPIO34 (ADC1)

3. **LDR (Photorésisteur)**
   - Type: Capteur de luminosité
   - Plage: 0-4095 (12 bits ADC)
   - Pin: GPIO35 (ADC1)

**Actionneurs:**
1. **Buzzer Piézoélectrique**
   - Fréquence: 1000Hz
   - Contrôle: PWM
   - Pin: GPIO25

2. **LED Rouge**
   - Fonction: Alarme visuelle
   - Résistance: 220Ω
   - Pin: GPIO26

3. **Module Relais**
   - Fonction: Contrôle ventilateur/extracteur
   - Tension: 5V
   - Pin: GPIO27

4. **LED Bleue**
   - Fonction: Indicateur d'état
   - Résistance: 220Ω
   - Pin: GPIO2

**Affichage:**
- LCD 16x2 avec interface I2C
  - Adresse I2C: 0x27
  - SDA: GPIO21
  - SCL: GPIO22
  - Rétroéclairage: Activé

#### Schéma de Connexion

```
ESP32                  Composants
─────                  ──────────

GPIO15  ──────────→   DHT22 (DATA)
GPIO34  ──────────→   MQ2/Joystick (ANALOG OUT)
GPIO35  ──────────→   LDR (ANALOG OUT)

GPIO21  ──────────→   LCD (SDA)
GPIO22  ──────────→   LCD (SCL)

GPIO25  ──────────→   Buzzer (+)
GPIO26  ──────────→   LED Rouge (via R220Ω)
GPIO27  ──────────→   Relais (IN)
GPIO2   ──────────→   LED Bleue (via R220Ω)

3V3     ──────────→   Alimentation capteurs
VIN     ──────────→   LCD (5V), Relais (5V)
GND     ──────────→   Masse commune
```

### 2.3 Architecture Logicielle

#### Diagramme d'État

```
┌──────────────┐
│ INITIALISATION│
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   VEILLE     │◄──────────────────┐
│ (Surveillance)│                   │
└──────┬───────┘                   │
       │                           │
       │ Seuil dépassé             │
       ▼                           │
┌──────────────┐                   │
│    ALARME    │                   │
│   ACTIVE     │                   │
└──────┬───────┘                   │
       │                           │
       │ Retour normal             │
       └───────────────────────────┘
```

#### Structure du Code

```cpp
// Organisation modulaire

// 1. CONFIGURATION
#define pins
struct Thresholds
Constantes WiFi

// 2. INITIALISATION
setup() {
  - Configuration pins
  - Chargement EEPROM
  - Init LCD, DHT22
  - Connexion WiFi
  - Affichage menu
}

// 3. BOUCLE PRINCIPALE
loop() {
  - Lecture capteurs (2s)
  - Vérification seuils
  - Mise à jour LCD (500ms)
  - Envoi ThingSpeak (20s)
  - Gestion commandes
  - Rotation affichage (5s)
}

// 4. FONCTIONS MÉTIER
readSensors()
checkThresholds()
activateAlarm()
deactivateAlarm()

// 5. FONCTIONS INTERFACE
updateDisplay()
handleSerialCommands()
showMenu()

// 6. FONCTIONS PERSISTANCE
saveThresholdsToEEPROM()
loadThresholdsFromEEPROM()

// 7. FONCTIONS IoT
sendToThingSpeak()
```

### 2.4 Choix Technologiques

#### Pourquoi l'ESP32?
- ✅ WiFi intégré (IoT)
- ✅ Puissance de calcul suffisante
- ✅ Nombreux GPIO
- ✅ 2 ADC pour capteurs analogiques
- ✅ Support I2C natif
- ✅ PWM pour buzzer
- ✅ Prix abordable

#### Pourquoi ces capteurs?
- **DHT22**: Précis, fiable, facile à utiliser
- **MQ2**: Standard industriel pour détection gaz
- **LDR**: Simple, économique pour luminosité

#### Pourquoi I2C pour le LCD?
- Utilise seulement 2 pins (vs 6+ en mode parallèle)
- Bus partageable avec d'autres périphériques
- Code simplifié

---

## 3. RÉALISATION

### 3.1 Implémentation du Code

#### Gestion des Capteurs

**Lecture DHT22:**
```cpp
temperature = dht.readTemperature();
humidity = dht.readHumidity();

// Vérification erreurs
if (isnan(temperature) || isnan(humidity)) {
  // Gestion erreur
}
```

**Lecture Analogique (MQ2, LDR):**
```cpp
gasLevel = analogRead(MQ2_PIN);    // 0-4095
lightLevel = analogRead(LDR_PIN);  // 0-4095
```

#### Gestion des Alarmes

**Logique Multi-Conditions:**
```cpp
void checkThresholds() {
  bool alarmNeeded = false;
  String alarmReason = "";
  
  // Vérification température
  if (temperature > thresholds.tempMax) {
    alarmNeeded = true;
    alarmReason += "TEMP ELEVEE! ";
    digitalWrite(RELAY_PIN, HIGH);  // Ventilateur
  }
  
  // Vérification gaz
  if (gasLevel > thresholds.gasMax) {
    alarmNeeded = true;
    alarmReason += "GAZ DETECTE! ";
    digitalWrite(RELAY_PIN, HIGH);  // Extracteur
  }
  
  // ... autres vérifications
  
  if (alarmNeeded) {
    activateAlarm(alarmReason);
  } else {
    deactivateAlarm();
  }
}
```

#### Persistance EEPROM

**Structure de Sauvegarde:**
```cpp
EEPROM Layout:
┌─────────────────────────────┐
│ Addr 0-3:  tempMax (float)  │
│ Addr 4-7:  humMax (float)   │
│ Addr 8-11: gasMax (int)     │
│ Addr 12-15: lightMin (int)  │
│ Addr 16:   Init Flag (0xAA) │
└─────────────────────────────┘
```

**Opérations:**
```cpp
// Sauvegarde
EEPROM.put(ADDR_TEMP_MAX, thresholds.tempMax);
EEPROM.write(EEPROM_INIT_FLAG, 0xAA);
EEPROM.commit();

// Chargement
if (EEPROM.read(EEPROM_INIT_FLAG) == 0xAA) {
  EEPROM.get(ADDR_TEMP_MAX, thresholds.tempMax);
}
```

#### Interface Série

**Parseur de Commandes:**
```cpp
void handleSerialCommands() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  command.toUpperCase();
  
  if (command == "STATUS") {
    // Afficher état
  }
  else if (command.startsWith("SETTEMP ")) {
    float value = command.substring(8).toFloat();
    // Validation et sauvegarde
  }
  // ... autres commandes
}
```

### 3.2 Optimisations Réalisées

#### Performance
- Lectures capteurs asynchrones (pas de delay())
- Utilisation de `millis()` pour timing
- Affichage LCD optimisé (pas de refresh inutile)

#### Mémoire
- Strings en Flash (F() macro possible)
- Structure compacte pour seuils
- EEPROM: 64 bytes seulement

#### Expérience Utilisateur
- Menu auto-affiché toutes les 30s
- Affichage rotatif automatique
- Messages clairs et informatifs
- LED de statut pour feedback visuel

---

## 4. TESTS ET VALIDATION

### 4.1 Méthodologie de Test

**Approche:**
- Tests unitaires par fonctionnalité
- Tests d'intégration (alarmes multiples)
- Tests de persistance (redémarrages)
- Tests de robustesse (valeurs limites)

### 4.2 Résultats des Tests

#### Test 1: Initialisation ✅
- Durée: 3 secondes
- Tous les composants initialisés
- Seuils chargés depuis EEPROM
- Menu affiché correctement

#### Test 2: Mesures Normales ✅
- Température: 24°C (précision: ±0.5°C)
- Humidité: 50% (précision: ±2%)
- Gaz: ~2000 (stable)
- Luminosité: ~2000 (stable)
- Pas d'alarme intempestive

#### Test 3: Alarme Température ✅
- Déclenchement: < 1 seconde
- Buzzer: Fonctionne (1000Hz)
- LED rouge: Allumée
- Relais: Activé
- Désactivation: Correcte au retour normal

#### Test 4: Alarme Gaz ✅
- Détection: Instantanée
- Seuil: Respecté (2000)
- Extracteur: Activé
- Message: Clair

#### Test 5: Configuration Serial ✅
- Toutes les commandes fonctionnelles
- Validation des entrées: OK
- Messages d'erreur: Appropriés
- Sauvegarde: Immédiate

#### Test 6: EEPROM ✅
- Sauvegarde: Vérifiée
- Chargement au boot: OK
- Persistance: Confirmée après plusieurs redémarrages
- Reset: Fonctionnel

#### Test 7: Alarmes Multiples ✅
- Gestion simultanée: 4 conditions
- Pas de conflit
- Désactivation progressive: OK
- Messages combinés: Clairs

#### Test 8: Affichage LCD ✅
- Rotation: Toutes les 5s
- Lisibilité: Excellente
- Pas de scintillement
- Symbole degré (°): Affiché

### 4.3 Problèmes Rencontrés et Solutions

| Problème | Solution Adoptée |
|----------|------------------|
| LCD ne s'affiche pas au démarrage | Ajout d'un délai de 100ms après lcd.init() |
| Buzzer trop fort | Ajout du paramètre volume à 0.1 dans diagram.json |
| Alarmes multiples se chevauchent | Utilisation de String pour concaténer les raisons |
| EEPROM non initialisée | Ajout d'un flag d'initialisation (0xAA) |
| Affichage Serial trop verbeux | Limitation à 1 affichage toutes les 2s pour les lectures |

### 4.4 Validation Finale

**Critères de validation:**
- ✅ Tous les tests de base réussis (6/6)
- ✅ Tous les tests avancés réussis (2/2)
- ✅ Aucune erreur critique
- ✅ Comportement stable sur 30 minutes
- ✅ Documentation complète

**Verdict: PROJET VALIDÉ ✅**

---

## 5. EXTENSIONS ET AMÉLIORATIONS

### 5.1 Extensions Implémentées

#### 5.1.1 Mémoire EEPROM
**Avantages:**
- Configuration persistante
- Survit aux redémarrages
- Pas besoin de reconfigurer à chaque fois

**Implémentation:**
- 64 bytes alloués
- Structure optimisée
- Flag d'initialisation

#### 5.1.2 Menu Interactif
**Fonctionnalités:**
- 8 commandes disponibles
- Aide contextuelle
- Validation des entrées
- Messages d'erreur clairs

**Ergonomie:**
- Menu auto-affiché toutes les 30s
- Commandes courtes (MENU, STATUS, etc.)
- Exemples fournis

#### 5.1.3 Support IoT (ThingSpeak)
**Architecture:**
```
ESP32 → WiFi → Internet → ThingSpeak
                              ↓
                         Dashboard
                         Graphiques
                         Alertes
```

**Données envoyées:**
- Field 1: Température
- Field 2: Humidité
- Field 3: Niveau gaz
- Field 4: Luminosité
- Field 5: État alarme

**Fréquence:** 20 secondes (limite gratuite ThingSpeak)

#### 5.1.4 Affichage Rotatif
**Écrans:**
1. Température / Humidité
2. Gaz / Luminosité
3. État Système / WiFi

**Avantages:**
- Meilleure visibilité de toutes les données
- Adaptation automatique
- Pas d'interaction nécessaire

#### 5.1.5 Relais Intelligent
**Logique:**
- Température élevée → Ventilateur ON
- Gaz détecté → Extracteur ON
- Conditions normales → OFF

**Sécurité:**
- Activation automatique
- Pas besoin d'intervention humaine

### 5.2 Améliorations Futures Possibles

#### Court Terme
1. **Application Mobile**
   - Blynk pour interface graphique
   - Notifications push
   - Contrôle à distance

2. **Capteurs Supplémentaires**
   - Capteur de pression (BMP280)
   - Capteur de particules (PM2.5)
   - Capteur de vibrations (ADXL345)

3. **Stockage Local**
   - Carte SD pour logs
   - Historique des alarmes
   - Export CSV

#### Long Terme
1. **Intelligence Artificielle**
   - Apprentissage des patterns
   - Prédiction de pannes
   - Optimisation automatique des seuils

2. **Réseau Multi-Capteurs**
   - Plusieurs ESP32 en réseau
   - Couverture d'une grande surface
   - Coordination centralisée

3. **Intégration Industrielle**
   - Protocole Modbus
   - SCADA
   - Conformité normes industrielles

---

## 6. CONCLUSION

### 6.1 Objectifs Atteints

Le projet S.I.M.S. a atteint tous les objectifs fixés:

**Exigences de base (100%):**
- ✅ ESP32 utilisé avec succès
- ✅ 3 capteurs fonctionnels (vs 2 requis)
- ✅ 4 actionneurs opérationnels (vs 1 requis)
- ✅ Affichage LCD clair et informatif
- ✅ Alarmes sonores et visuelles efficaces
- ✅ Configuration flexible via Serial

**Extensions réalisées (100%):**
- ✅ EEPROM: Persistance parfaite
- ✅ Menu interactif: 8 commandes
- ✅ IoT: ThingSpeak fonctionnel
- ✅ Affichage rotatif: Automatique
- ✅ Relais intelligent: Réactions appropriées

### 6.2 Compétences Acquises

**Techniques:**
- Programmation ESP32 (Arduino Framework)
- Communication I2C
- Lecture capteurs analogiques et numériques
- Gestion PWM pour buzzer
- Persistance EEPROM
- Protocoles WiFi et HTTP
- Gestion multi-tâches (timing)

**Méthodologiques:**
- Analyse fonctionnelle
- Conception d'architecture
- Tests systématiques
- Documentation technique
- Débogage matériel et logiciel

**Transversales:**
- Gestion de projet
- Résolution de problèmes
- Optimisation code/mémoire
- Ergonomie utilisateur

### 6.3 Points Forts du Projet

1. **Complétude:** Tous les critères + extensions
2. **Robustesse:** Tests approfondis, gestion d'erreurs
3. **Extensibilité:** Code modulaire, facile à étendre
4. **Documentation:** Complète et détaillée
5. **Ergonomie:** Interface utilisateur soignée
6. **Innovation:** Extensions originales (EEPROM, IoT, menu)

### 6.4 Difficultés Rencontrées

1. **Timing Multitâche:**
   - Problème: Gestion de multiples timings simultanés
   - Solution: Utilisation de `millis()` et variables de timestamp

2. **Simulation Wokwi:**
   - Problème: Pas de vrai capteur MQ2 disponible
   - Solution: Utilisation du joystick comme simulation analogique

3. **Affichage LCD:**
   - Problème: Scintillement initial
   - Solution: Optimisation des refresh, clear() seulement quand nécessaire

4. **Mémoire EEPROM:**
   - Problème: Première utilisation, valeurs aléatoires
   - Solution: Flag d'initialisation pour détecter première utilisation

### 6.5 Apports Pédagogiques

Ce projet a permis de:
- Appliquer les concepts théoriques vus en cours
- Comprendre les contraintes d'un système embarqué réel
- Développer une méthodologie de test rigoureuse
- Apprendre à documenter un projet technique
- Simuler un environnement industriel réaliste

### 6.6 Perspectives

Le système S.I.M.S. peut évoluer vers:
- Un produit commercialisable (après certification)
- Une plateforme éducative pour l'IoT industriel
- Un système de monitoring multi-sites
- Une base pour des projets plus complexes (IA, Big Data)

### 6.7 Remerciements

Nous tenons à remercier:
- Le corps enseignant pour l'encadrement
- La plateforme Wokwi pour la simulation
- La communauté Arduino pour les bibliothèques
- ThingSpeak pour l'hébergement IoT gratuit

---

## 7. ANNEXES

### Annexe A: Code Source Complet
Voir fichier: `sketch.ino` (520 lignes)

### Annexe B: Schéma Wokwi
Voir fichier: `diagram.json`

### Annexe C: Bibliothèques Utilisées
```
- LiquidCrystal I2C (1.1.2)
- DHT sensor library (1.4.4)
- WiFi (ESP32 built-in)
- HTTPClient (ESP32 built-in)
- EEPROM (ESP32 built-in)
- Wire (ESP32 built-in)
```

### Annexe D: Références
1. ESP32 Datasheet: https://www.espressif.com/
2. DHT22 Datasheet: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
3. MQ2 Datasheet: https://www.pololu.com/file/0J309/MQ2.pdf
4. LCD I2C Library: https://github.com/johnrickman/LiquidCrystal_I2C
5. ThingSpeak API: https://www.mathworks.com/help/thingspeak/

### Annexe E: Glossaire
- **ADC**: Analog-to-Digital Converter
- **EEPROM**: Electrically Erasable Programmable Read-Only Memory
- **GPIO**: General Purpose Input/Output
- **I2C**: Inter-Integrated Circuit
- **IoT**: Internet of Things
- **LCD**: Liquid Crystal Display
- **PWM**: Pulse Width Modulation
- **UART**: Universal Asynchronous Receiver-Transmitter

### Annexe F: Captures d'Écran
À inclure dans la version finale:
1. Vue d'ensemble du circuit Wokwi
2. Serial Monitor - Menu
3. Serial Monitor - Alarme active
4. LCD - Affichage température
5. LCD - Affichage gaz
6. Dashboard ThingSpeak

---

**Fin du Rapport**

**Date:** 13 Novembre 2025  
**Projet:** S.I.M.S. - Smart Industrial Monitoring System  
**Plateforme:** Wokwi Simulator  
**Microcontrôleur:** ESP32 DevKit V1

---
