/*
 * Smart Industrial Monitoring System (S.I.M.S.)
 * Système de surveillance d'environnement industriel avec ESP32
 * 
 * Capteurs:
 * - DHT22: Température et humidité
 * - MQ2: Détection de gaz (GPL, CO, fumée)
 * - LDR: Capteur de luminosité
 * 
 * Actionneurs:
 * - Buzzer: Alarme sonore
 * - LED Rouge: Alarme visuelle
 * - Relais: Contrôle ventilateur/extracteur
 * 
 * Affichage: LCD I2C 16x2
 * IoT: ThingSpeak pour monitoring à distance
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ============ Configuration des pins ============
#define DHT_PIN 15          // DHT22 sur GPIO15
#define DHT_TYPE DHT22
#define MQ2_PIN 34          // MQ2 (analogique) sur GPIO34
#define LDR_PIN 35          // LDR (analogique) sur GPIO35
#define BUZZER_PIN 25       // Buzzer sur GPIO25
#define LED_ALARM_PIN 26    // LED Rouge alarme sur GPIO26
#define RELAY_PIN 27        // Relais sur GPIO27
#define LED_STATUS_PIN 2    // LED bleue status sur GPIO2

// ============ Configuration LCD I2C ============
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adresse I2C: 0x27, 16 colonnes, 2 lignes

// ============ Configuration DHT22 ============
DHT dht(DHT_PIN, DHT_TYPE);

// ============ Configuration WiFi et ThingSpeak ============
const char* ssid = "Wokwi-GUEST";           // WiFi pour Wokwi
const char* password = "";                   // Pas de mot de passe pour Wokwi-GUEST
String thingSpeakApiKey = "YOUR_API_KEY";    // Remplacer par votre clé API
const char* thingSpeakServer = "http://api.thingspeak.com/update";

// ============ Configuration MQTT ============
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
String mqttClientId = "sims-esp32-";  // Will be completed with MAC address

// Topics MQTT
const char* topicSensorsData = "sims/sensors/data";
const char* topicStatus = "sims/status";
const char* topicControlRelay = "sims/control/relay";
const char* topicControlAlarm = "sims/control/alarm";
const char* topicControlThresholds = "sims/control/thresholds";
const char* topicControlRefresh = "sims/control/refresh";

// Clients MQTT et Web
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WebServer webServer(80);

// ============ Seuils par défaut ============
struct Thresholds {
  float tempMax;      // Température maximale (°C)
  float humidityMax;  // Humidité maximale (%)
  int gasMax;         // Niveau de gaz maximal (0-4095)
  int lightMin;       // Luminosité minimale (0-4095)
};

Thresholds thresholds = {30.0, 70.0, 2000, 500};  // Valeurs par défaut

// ============ Variables globales ============
float temperature = 0;
float humidity = 0;
int gasLevel = 0;
int lightLevel = 0;
bool alarmActive = false;
bool manualRelayControl = false;
bool manualAlarmControl = false;
unsigned long lastReadTime = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastThingSpeakUpdate = 0;
unsigned long lastSerialMenu = 0;
unsigned long lastMqttPublish = 0;
int displayMode = 0;  // 0: Temp/Hum, 1: Gas/Light, 2: Status

// ============ Adresses EEPROM ============
#define EEPROM_SIZE 64
#define ADDR_TEMP_MAX 0
#define ADDR_HUM_MAX 4
#define ADDR_GAS_MAX 8
#define ADDR_LIGHT_MIN 12
#define EEPROM_INIT_FLAG 16

// ============ Prototypes de fonctions ============
void readSensors();
void checkThresholds();
void updateDisplay();
void activateAlarm(String reason);
void deactivateAlarm();
void saveThresholdsToEEPROM();
void loadThresholdsFromEEPROM();
void handleSerialCommands();
void sendToThingSpeak();
void showMenu();
void blinkStatusLED();
void connectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishSensorData();
void setupWebServer();
void handleRoot();
void handleStatus();
void handleSensors();
void handleNotFound();

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=================================");
  Serial.println("S.I.M.S. - Smart Industrial Monitoring System");
  Serial.println("Initialisation...");
  Serial.println("=================================\n");
  
  // Initialisation des pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_ALARM_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_STATUS_PIN, OUTPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  // Désactivation des alarmes au démarrage
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_ALARM_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  
  // Initialisation de l'EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadThresholdsFromEEPROM();
  
  // Initialisation du LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  S.I.M.S. v1.0");
  lcd.setCursor(0, 1);
  lcd.print(" Initialisation");
  delay(2000);
  
  // Initialisation du DHT22
  dht.begin();
  Serial.println("[OK] DHT22 initialisé");
  
  // Initialisation WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connect...");
  Serial.print("Connexion WiFi...");
  WiFi.begin(ssid, password);
  
  int wifiTimeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout < 20) {
    delay(500);
    Serial.print(".");
    wifiTimeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connecté!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("WiFi: OK        ");
  } else {
    Serial.println(" Échec!");
    lcd.setCursor(0, 1);
    lcd.print("WiFi: FAIL      ");
  }
  delay(2000);
  
  // Initialisation MQTT
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQTT Connect...");
    
    // Générer un ID client unique basé sur l'adresse MAC
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "");  // Retirer les deux-points
    mqttClientId += macAddr;
    
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);
    connectMQTT();
    delay(1000);
  }
  
  // Initialisation du serveur Web
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Web Server...");
    setupWebServer();
    webServer.begin();
    Serial.println("[OK] Serveur Web démarré");
    Serial.print("URL: http://");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Web: OK         ");
    delay(2000);
  }
  
  // Affichage des seuils configurés
  Serial.println("\n--- Seuils configurés ---");
  Serial.printf("Température max: %.1f°C\n", thresholds.tempMax);
  Serial.printf("Humidité max: %.1f%%\n", thresholds.humidityMax);
  Serial.printf("Gaz max: %d\n", thresholds.gasMax);
  Serial.printf("Luminosité min: %d\n", thresholds.lightMin);
  Serial.println("-------------------------\n");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Systeme pret!");
  lcd.setCursor(0, 1);
  lcd.print("Surveillance...");
  delay(2000);
  
  // Affichage du menu
  showMenu();
  
  Serial.println("\n[SYSTÈME PRÊT] Surveillance active...\n");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Maintenir la connexion MQTT
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      connectMQTT();
    }
    mqttClient.loop();
  }
  
  // Gérer les requêtes Web
  webServer.handleClient();
  
  // Lecture des capteurs toutes les 2 secondes
  if (currentTime - lastReadTime >= 2000) {
    lastReadTime = currentTime;
    readSensors();
    checkThresholds();
  }
  
  // Mise à jour de l'affichage toutes les 500ms
  if (currentTime - lastDisplayUpdate >= 500) {
    lastDisplayUpdate = currentTime;
    updateDisplay();
    blinkStatusLED();
  }
  
  // Publication MQTT toutes les 5 secondes
  if (WiFi.status() == WL_CONNECTED && mqttClient.connected() && currentTime - lastMqttPublish >= 5000) {
    lastMqttPublish = currentTime;
    publishSensorData();
  }
  
  // Envoi vers ThingSpeak toutes les 20 secondes
  if (WiFi.status() == WL_CONNECTED && currentTime - lastThingSpeakUpdate >= 20000) {
    lastThingSpeakUpdate = currentTime;
    sendToThingSpeak();
  }
  
  // Affichage du menu toutes les 30 secondes
  if (currentTime - lastSerialMenu >= 30000) {
    lastSerialMenu = currentTime;
    showMenu();
  }
  
  // Gestion des commandes série
  handleSerialCommands();
  
  // Rotation de l'affichage toutes les 5 secondes
  if (currentTime % 5000 < 500 && currentTime - lastDisplayUpdate < 500) {
    displayMode = (displayMode + 1) % 3;
  }
}

void readSensors() {
  // Lecture DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Vérification des erreurs de lecture DHT22
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[ERREUR] Lecture DHT22 échouée!");
    temperature = 0;
    humidity = 0;
  }
  
  // Lecture MQ2 (analogique)
  gasLevel = analogRead(MQ2_PIN);
  
  // Lecture LDR (analogique)
  lightLevel = analogRead(LDR_PIN);
  
  // Affichage des valeurs dans le Serial Monitor
  Serial.println("--- Lectures capteurs ---");
  Serial.printf("Température: %.1f°C\n", temperature);
  Serial.printf("Humidité: %.1f%%\n", humidity);
  Serial.printf("Niveau gaz: %d (0-4095)\n", gasLevel);
  Serial.printf("Luminosité: %d (0-4095)\n", lightLevel);
  Serial.println("------------------------\n");
}

void checkThresholds() {
  bool alarmNeeded = false;
  String alarmReason = "";
  
  // Vérification température
  if (temperature > thresholds.tempMax && temperature > 0) {
    alarmNeeded = true;
    alarmReason += "TEMP ELEVEE! ";
    if (!manualRelayControl) {
      digitalWrite(RELAY_PIN, HIGH);  // Activation ventilateur
    }
  }
  
  // Vérification humidité
  if (humidity > thresholds.humidityMax && humidity > 0) {
    alarmNeeded = true;
    alarmReason += "HUM ELEVEE! ";
  }
  
  // Vérification gaz
  if (gasLevel > thresholds.gasMax) {
    alarmNeeded = true;
    alarmReason += "GAZ DETECTE! ";
    if (!manualRelayControl) {
      digitalWrite(RELAY_PIN, HIGH);  // Activation extracteur
    }
  }
  
  // Vérification luminosité
  if (lightLevel < thresholds.lightMin) {
    alarmNeeded = true;
    alarmReason += "LUMIERE FAIBLE! ";
  }
  
  // Activation/Désactivation de l'alarme
  if (manualAlarmControl) {
    // Mode manuel - ne pas changer l'état
  } else if (alarmNeeded) {
    activateAlarm(alarmReason);
  } else {
    deactivateAlarm();
  }
}

void activateAlarm(String reason) {
  if (!alarmActive) {
    Serial.println("\n!!! ALARME ACTIVÉE !!!");
    Serial.println("Raison: " + reason);
    Serial.println("!!!!!!!!!!!!!!!!!!!!!\n");
  }
  
  alarmActive = true;
  
  // Activation buzzer (tonalité intermittente)
  tone(BUZZER_PIN, 1000, 200);
  
  // Activation LED alarme
  digitalWrite(LED_ALARM_PIN, HIGH);
}

void deactivateAlarm() {
  if (alarmActive) {
    Serial.println("[INFO] Conditions normales rétablies. Alarme désactivée.\n");
  }
  
  if (!manualAlarmControl) {
    alarmActive = false;
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_ALARM_PIN, LOW);
  }
  
  if (!manualRelayControl) {
    digitalWrite(RELAY_PIN, LOW);
  }
}

void updateDisplay() {
  lcd.clear();
  
  switch (displayMode) {
    case 0:  // Température et Humidité
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperature, 1);
      lcd.print((char)223);  // Symbole degré
      lcd.print("C");
      
      lcd.setCursor(0, 1);
      lcd.print("Hum:  ");
      lcd.print(humidity, 1);
      lcd.print("%");
      break;
      
    case 1:  // Gaz et Luminosité
      lcd.setCursor(0, 0);
      lcd.print("Gaz:  ");
      lcd.print(gasLevel);
      
      lcd.setCursor(0, 1);
      lcd.print("Lum:  ");
      lcd.print(lightLevel);
      break;
      
    case 2:  // Status système
      lcd.setCursor(0, 0);
      if (alarmActive) {
        lcd.print("!!! ALARME !!!");
      } else {
        lcd.print("Systeme: OK");
      }
      
      lcd.setCursor(0, 1);
      if (WiFi.status() == WL_CONNECTED) {
        lcd.print("WiFi: Connecte");
      } else {
        lcd.print("WiFi: Deconnect");
      }
      break;
  }
}

void blinkStatusLED() {
  static bool ledState = false;
  static unsigned long lastBlink = 0;
  
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(LED_STATUS_PIN, ledState ? HIGH : LOW);
  }
}

void sendToThingSpeak() {
  if (thingSpeakApiKey == "YOUR_API_KEY") {
    // Serial.println("[INFO] ThingSpeak non configuré (remplacer YOUR_API_KEY)");
    return;
  }
  
  HTTPClient http;
  
  String url = String(thingSpeakServer) + "?api_key=" + thingSpeakApiKey;
  url += "&field1=" + String(temperature);
  url += "&field2=" + String(humidity);
  url += "&field3=" + String(gasLevel);
  url += "&field4=" + String(lightLevel);
  url += "&field5=" + String(alarmActive ? 1 : 0);
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.println("[ThingSpeak] Données envoyées avec succès!");
  } else {
    Serial.printf("[ThingSpeak] Erreur: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void saveThresholdsToEEPROM() {
  EEPROM.put(ADDR_TEMP_MAX, thresholds.tempMax);
  EEPROM.put(ADDR_HUM_MAX, thresholds.humidityMax);
  EEPROM.put(ADDR_GAS_MAX, thresholds.gasMax);
  EEPROM.put(ADDR_LIGHT_MIN, thresholds.lightMin);
  EEPROM.write(EEPROM_INIT_FLAG, 0xAA);  // Flag d'initialisation
  EEPROM.commit();
  
  Serial.println("[EEPROM] Seuils sauvegardés!");
}

void loadThresholdsFromEEPROM() {
  if (EEPROM.read(EEPROM_INIT_FLAG) == 0xAA) {
    // EEPROM déjà initialisée, charger les valeurs
    EEPROM.get(ADDR_TEMP_MAX, thresholds.tempMax);
    EEPROM.get(ADDR_HUM_MAX, thresholds.humidityMax);
    EEPROM.get(ADDR_GAS_MAX, thresholds.gasMax);
    EEPROM.get(ADDR_LIGHT_MIN, thresholds.lightMin);
    Serial.println("[EEPROM] Seuils chargés depuis la mémoire");
  } else {
    // Première utilisation, sauvegarder les valeurs par défaut
    saveThresholdsToEEPROM();
    Serial.println("[EEPROM] Seuils par défaut sauvegardés");
  }
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    if (command == "MENU" || command == "M") {
      showMenu();
    }
    else if (command == "STATUS" || command == "S") {
      Serial.println("\n=== STATUS SYSTÈME ===");
      Serial.printf("Température: %.1f°C (Max: %.1f°C)\n", temperature, thresholds.tempMax);
      Serial.printf("Humidité: %.1f%% (Max: %.1f%%)\n", humidity, thresholds.humidityMax);
      Serial.printf("Gaz: %d (Max: %d)\n", gasLevel, thresholds.gasMax);
      Serial.printf("Luminosité: %d (Min: %d)\n", lightLevel, thresholds.lightMin);
      Serial.printf("Alarme: %s\n", alarmActive ? "ACTIVE" : "Inactive");
      Serial.printf("WiFi: %s\n", WiFi.status() == WL_CONNECTED ? "Connecté" : "Déconnecté");
      Serial.println("=====================\n");
    }
    else if (command.startsWith("SETTEMP ")) {
      float newTemp = command.substring(8).toFloat();
      if (newTemp > 0 && newTemp < 100) {
        thresholds.tempMax = newTemp;
        saveThresholdsToEEPROM();
        Serial.printf("Seuil température mis à jour: %.1f°C\n", newTemp);
      } else {
        Serial.println("Erreur: Valeur invalide (0-100)");
      }
    }
    else if (command.startsWith("SETHUM ")) {
      float newHum = command.substring(7).toFloat();
      if (newHum > 0 && newHum <= 100) {
        thresholds.humidityMax = newHum;
        saveThresholdsToEEPROM();
        Serial.printf("Seuil humidité mis à jour: %.1f%%\n", newHum);
      } else {
        Serial.println("Erreur: Valeur invalide (0-100)");
      }
    }
    else if (command.startsWith("SETGAS ")) {
      int newGas = command.substring(7).toInt();
      if (newGas >= 0 && newGas <= 4095) {
        thresholds.gasMax = newGas;
        saveThresholdsToEEPROM();
        Serial.printf("Seuil gaz mis à jour: %d\n", newGas);
      } else {
        Serial.println("Erreur: Valeur invalide (0-4095)");
      }
    }
    else if (command.startsWith("SETLIGHT ")) {
      int newLight = command.substring(9).toInt();
      if (newLight >= 0 && newLight <= 4095) {
        thresholds.lightMin = newLight;
        saveThresholdsToEEPROM();
        Serial.printf("Seuil luminosité mis à jour: %d\n", newLight);
      } else {
        Serial.println("Erreur: Valeur invalide (0-4095)");
      }
    }
    else if (command == "RESET") {
      thresholds = {30.0, 70.0, 2000, 500};
      saveThresholdsToEEPROM();
      Serial.println("Seuils réinitialisés aux valeurs par défaut");
    }
    else if (command == "HELP" || command == "H") {
      showMenu();
    }
    else {
      Serial.println("Commande inconnue. Tapez MENU pour voir les commandes disponibles.");
    }
  }
}

void showMenu() {
  Serial.println("\n╔════════════════════════════════════════════════════════╗");
  Serial.println("║     S.I.M.S. - MENU DE CONFIGURATION                  ║");
  Serial.println("╠════════════════════════════════════════════════════════╣");
  Serial.println("║ Commandes disponibles:                                ║");
  Serial.println("║                                                        ║");
  Serial.println("║  MENU ou M       - Afficher ce menu                   ║");
  Serial.println("║  STATUS ou S     - Afficher l'état du système         ║");
  Serial.println("║  SETTEMP <val>   - Définir seuil température (°C)     ║");
  Serial.println("║  SETHUM <val>    - Définir seuil humidité (%)         ║");
  Serial.println("║  SETGAS <val>    - Définir seuil gaz (0-4095)         ║");
  Serial.println("║  SETLIGHT <val>  - Définir seuil luminosité (0-4095)  ║");
  Serial.println("║  RESET           - Réinitialiser aux valeurs défaut    ║");
  Serial.println("║  HELP ou H       - Afficher l'aide                    ║");
  Serial.println("║                                                        ║");
  Serial.println("║ Exemples:                                              ║");
  Serial.println("║  SETTEMP 35      - Température max = 35°C             ║");
  Serial.println("║  SETHUM 80       - Humidité max = 80%                 ║");
  Serial.println("║  SETGAS 2500     - Niveau gaz max = 2500              ║");
  Serial.println("╚════════════════════════════════════════════════════════╝");
  Serial.println();
}

// ============ Fonctions MQTT ============

void connectMQTT() {
  if (mqttClient.connected()) {
    return;
  }
  
  Serial.print("Connexion au broker MQTT...");
  
  if (mqttClient.connect(mqttClientId.c_str())) {
    Serial.println(" Connecté!");
    
    // Souscription aux topics de contrôle
    mqttClient.subscribe(topicControlRelay);
    mqttClient.subscribe(topicControlAlarm);
    mqttClient.subscribe(topicControlThresholds);
    mqttClient.subscribe(topicControlRefresh);
    
    Serial.println("[MQTT] Souscription aux topics de contrôle OK");
    
    // Publier un message de statut
    mqttClient.publish(topicStatus, "{\"status\":\"online\"}");
  } else {
    Serial.print(" Échec! Code d'erreur: ");
    int state = mqttClient.state();
    Serial.print(state);
    Serial.print(" - ");
    // Messages d'erreur descriptifs
    switch (state) {
      case -4: Serial.println("Timeout de connexion"); break;
      case -3: Serial.println("Connexion perdue"); break;
      case -2: Serial.println("Échec de connexion"); break;
      case -1: Serial.println("Client déconnecté"); break;
      case 1: Serial.println("Mauvais protocole"); break;
      case 2: Serial.println("Client ID rejeté"); break;
      case 3: Serial.println("Serveur indisponible"); break;
      case 4: Serial.println("Mauvais identifiants"); break;
      case 5: Serial.println("Non autorisé"); break;
      default: Serial.println("Erreur inconnue"); break;
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convertir le payload en String
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.printf("[MQTT] Message reçu sur %s: %s\n", topic, message.c_str());
  
  // Parser JSON avec buffer adapté
  StaticJsonDocument<256> doc;  // Augmenté à 256 bytes pour plus de sécurité
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.println("[MQTT] Erreur parsing JSON");
    return;
  }
  
  // Traitement selon le topic
  if (strcmp(topic, topicControlRelay) == 0) {
    const char* state = doc["state"];
    if (state != nullptr) {
      if (strcmp(state, "ON") == 0) {
        digitalWrite(RELAY_PIN, HIGH);
        manualRelayControl = true;
        Serial.println("[MQTT] Relais activé manuellement");
      } else if (strcmp(state, "OFF") == 0) {
        digitalWrite(RELAY_PIN, LOW);
        manualRelayControl = false;
        Serial.println("[MQTT] Relais désactivé");
      }
    } else {
      Serial.println("[MQTT] Erreur: état du relais non spécifié");
    }
  }
  else if (strcmp(topic, topicControlAlarm) == 0) {
    const char* state = doc["state"];
    if (state != nullptr) {
      if (strcmp(state, "ON") == 0) {
        manualAlarmControl = true;
        activateAlarm("ALARME MANUELLE");
        Serial.println("[MQTT] Alarme activée manuellement");
      } else if (strcmp(state, "OFF") == 0) {
        manualAlarmControl = false;
        deactivateAlarm();
        Serial.println("[MQTT] Alarme désactivée manuellement");
      }
    } else {
      Serial.println("[MQTT] Erreur: état de l'alarme non spécifié");
    }
  }
  else if (strcmp(topic, topicControlThresholds) == 0) {
    // Validation et mise à jour des seuils
    if (doc.containsKey("temp")) {
      float temp = doc["temp"];
      if (temp > 0 && temp <= 100) {
        thresholds.tempMax = temp;
      } else {
        Serial.println("[MQTT] Erreur: température invalide (0-100°C)");
      }
    }
    if (doc.containsKey("hum")) {
      float hum = doc["hum"];
      if (hum > 0 && hum <= 100) {
        thresholds.humidityMax = hum;
      } else {
        Serial.println("[MQTT] Erreur: humidité invalide (0-100%)");
      }
    }
    if (doc.containsKey("gas")) {
      int gas = doc["gas"];
      if (gas >= 0 && gas <= 4095) {
        thresholds.gasMax = gas;
      } else {
        Serial.println("[MQTT] Erreur: gaz invalide (0-4095)");
      }
    }
    if (doc.containsKey("light")) {
      int light = doc["light"];
      if (light >= 0 && light <= 4095) {
        thresholds.lightMin = light;
      } else {
        Serial.println("[MQTT] Erreur: luminosité invalide (0-4095)");
      }
    }
    saveThresholdsToEEPROM();
    Serial.println("[MQTT] Seuils mis à jour via MQTT");
  }
  else if (strcmp(topic, topicControlRefresh) == 0) {
    publishSensorData();
    Serial.println("[MQTT] Données rafraîchies");
  }
}

void publishSensorData() {
  if (!mqttClient.connected()) {
    return;
  }
  
  // Créer JSON avec les données des capteurs
  StaticJsonDocument<256> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["gas"] = gasLevel;
  doc["light"] = lightLevel;
  doc["alarm"] = alarmActive;
  doc["relay"] = digitalRead(RELAY_PIN);
  doc["wifi"] = WiFi.status() == WL_CONNECTED;
  doc["timestamp"] = millis() / 1000;
  
  String output;
  serializeJson(doc, output);
  
  mqttClient.publish(topicSensorsData, output.c_str());
}

// ============ Fonctions Web Server ============

void setupWebServer() {
  // Route principale - Page HTML
  webServer.on("/", handleRoot);
  
  // Routes API
  webServer.on("/api/status", handleStatus);
  webServer.on("/api/sensors", handleSensors);
  
  // Route pour les fichiers non trouvés
  webServer.onNotFound(handleNotFound);
}

void handleRoot() {
  // Interface web complète intégrée - accessible directement depuis l'ESP32 dans Wokwi
  // Plus besoin d'ouvrir un fichier séparé!
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>S.I.M.S. - Système de Surveillance Industrielle</title>
    
    <!-- Bootstrap 5 -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <!-- Font Awesome -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    
    <style>
        :root {
            --primary-dark: #1a2332;
            --primary-blue: #2c5f8d;
            --accent-orange: #ff6b35;
            --status-ok: #28a745;
            --status-warning: #ffc107;
            --status-danger: #dc3545;
        }
        
        body {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        
        .main-container {
            padding: 20px;
            max-width: 1400px;
            margin: 0 auto;
        }
        
        .header {
            background: var(--primary-dark);
            color: white;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        
        .header h1 {
            margin: 0;
            font-size: 2.5rem;
            font-weight: bold;
        }
        
        .status-badge {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.9rem;
            font-weight: bold;
        }
        
        .status-connected {
            background: var(--status-ok);
            color: white;
        }
        
        .status-disconnected {
            background: var(--status-danger);
            color: white;
        }
        
        .sensor-card {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            transition: transform 0.3s;
        }
        
        .sensor-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 6px 12px rgba(0,0,0,0.15);
        }
        
        .sensor-card .icon {
            font-size: 3rem;
            margin-bottom: 10px;
        }
        
        .sensor-card .value {
            font-size: 2.5rem;
            font-weight: bold;
            color: var(--primary-blue);
        }
        
        .sensor-card .label {
            font-size: 1.1rem;
            color: #666;
            margin-top: 5px;
        }
        
        .sensor-card.alarm {
            border: 3px solid var(--status-danger);
            animation: pulse 1s infinite;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.8; }
        }
        
        .control-card {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        
        .btn-control {
            width: 100%;
            padding: 15px;
            font-size: 1.1rem;
            font-weight: bold;
            border-radius: 8px;
            transition: all 0.3s;
        }
        
        .btn-control.active {
            background: var(--status-ok);
            border-color: var(--status-ok);
        }
        
        .btn-control.inactive {
            background: #6c757d;
            border-color: #6c757d;
        }
        
        .chart-container {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        
        .log-console {
            background: #1e1e1e;
            color: #00ff00;
            border-radius: 10px;
            padding: 15px;
            max-height: 300px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 0.9rem;
        }
        
        .log-entry {
            margin-bottom: 5px;
            padding: 2px 5px;
        }
        
        .log-entry.info { color: #00ff00; }
        .log-entry.warning { color: #ffc107; }
        .log-entry.error { color: #ff4444; }
        
        .config-form {
            background: white;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
    </style>
</head>
<body>
    <div class="main-container">
        <!-- En-tête -->
        <div class="header">
            <div class="d-flex justify-content-between align-items-center">
                <div>
                    <h1><i class="fas fa-industry"></i> S.I.M.S.</h1>
                    <p class="mb-0">Smart Industrial Monitoring System</p>
                </div>
                <div class="text-end">
                    <span id="mqttStatus" class="status-badge status-disconnected">
                        <i class="fas fa-circle-notch fa-spin"></i> Connexion...
                    </span>
                    <div class="mt-2">
                        <small id="lastUpdate">Dernière mise à jour: --</small>
                    </div>
                </div>
            </div>
        </div>

        <!-- Indicateurs temps réel -->
        <div class="row">
            <div class="col-md-3 col-sm-6">
                <div class="sensor-card" id="tempCard">
                    <div class="text-center">
                        <div class="icon"><i class="fas fa-thermometer-half" style="color: #e74c3c;"></i></div>
                        <div class="value" id="tempValue">--</div>
                        <div class="label">Température (°C)</div>
                        <small class="text-muted">Max: <span id="tempThreshold">--</span>°C</small>
                    </div>
                </div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="sensor-card" id="humCard">
                    <div class="text-center">
                        <div class="icon"><i class="fas fa-tint" style="color: #3498db;"></i></div>
                        <div class="value" id="humValue">--</div>
                        <div class="label">Humidité (%)</div>
                        <small class="text-muted">Max: <span id="humThreshold">--</span>%</small>
                    </div>
                </div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="sensor-card" id="gasCard">
                    <div class="text-center">
                        <div class="icon"><i class="fas fa-smog" style="color: #f39c12;"></i></div>
                        <div class="value" id="gasValue">--</div>
                        <div class="label">Niveau Gaz</div>
                        <small class="text-muted">Max: <span id="gasThreshold">--</span></small>
                    </div>
                </div>
            </div>
            <div class="col-md-3 col-sm-6">
                <div class="sensor-card" id="lightCard">
                    <div class="text-center">
                        <div class="icon"><i class="fas fa-lightbulb" style="color: #f1c40f;"></i></div>
                        <div class="value" id="lightValue">--</div>
                        <div class="label">Luminosité</div>
                        <small class="text-muted">Min: <span id="lightThreshold">--</span></small>
                    </div>
                </div>
            </div>
        </div>

        <!-- Graphiques -->
        <div class="row">
            <div class="col-lg-6">
                <div class="chart-container">
                    <h4><i class="fas fa-chart-line"></i> Température & Humidité</h4>
                    <canvas id="tempHumChart"></canvas>
                </div>
            </div>
            <div class="col-lg-6">
                <div class="chart-container">
                    <h4><i class="fas fa-chart-line"></i> Gaz & Luminosité</h4>
                    <canvas id="gasLightChart"></canvas>
                </div>
            </div>
        </div>

        <!-- Contrôles et Configuration -->
        <div class="row">
            <div class="col-lg-4">
                <div class="control-card">
                    <h4><i class="fas fa-sliders-h"></i> Contrôles</h4>
                    <div class="mb-3">
                        <button id="btnRelay" class="btn btn-control inactive" onclick="toggleRelay()">
                            <i class="fas fa-fan"></i> Relais: OFF
                        </button>
                    </div>
                    <div class="mb-3">
                        <button id="btnAlarm" class="btn btn-control inactive" onclick="toggleAlarm()">
                            <i class="fas fa-bell"></i> Alarme: OFF
                        </button>
                    </div>
                    <div>
                        <button class="btn btn-primary btn-control" onclick="refreshData()">
                            <i class="fas fa-sync-alt"></i> Rafraîchir
                        </button>
                    </div>
                </div>
            </div>
            
            <div class="col-lg-8">
                <div class="config-form">
                    <h4><i class="fas fa-cog"></i> Configuration des Seuils</h4>
                    <form id="thresholdForm" onsubmit="updateThresholds(event)">
                        <div class="row">
                            <div class="col-md-6 mb-3">
                                <label class="form-label">Température Max (°C)</label>
                                <input type="number" class="form-control" id="inputTemp" step="0.1" required>
                            </div>
                            <div class="col-md-6 mb-3">
                                <label class="form-label">Humidité Max (%)</label>
                                <input type="number" class="form-control" id="inputHum" step="0.1" required>
                            </div>
                            <div class="col-md-6 mb-3">
                                <label class="form-label">Gaz Max (0-4095)</label>
                                <input type="number" class="form-control" id="inputGas" min="0" max="4095" required>
                            </div>
                            <div class="col-md-6 mb-3">
                                <label class="form-label">Luminosité Min (0-4095)</label>
                                <input type="number" class="form-control" id="inputLight" min="0" max="4095" required>
                            </div>
                        </div>
                        <button type="submit" class="btn btn-success">
                            <i class="fas fa-save"></i> Enregistrer les Seuils
                        </button>
                    </form>
                </div>
            </div>
        </div>

        <!-- Console de Logs -->
        <div class="row mt-3">
            <div class="col-12">
                <div class="control-card">
                    <h4><i class="fas fa-terminal"></i> Console d'Événements</h4>
                    <div class="log-console" id="logConsole">
                        <div class="log-entry info">[INFO] Interface web chargée</div>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- Bootstrap JS -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <!-- Chart.js -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.3.0/dist/chart.umd.js"></script>
    <!-- MQTT.js -->
    <script src="https://unpkg.com/mqtt@5.0.0/dist/mqtt.min.js"></script>

    <script>
        // Variables globales
        let mqttClient = null;
        let tempHumChart = null;
        let gasLightChart = null;
        let sensorData = {
            temperature: [],
            humidity: [],
            gas: [],
            light: [],
            timestamps: []
        };
        const MAX_DATA_POINTS = 20;
        
        // Seuils actuels
        let currentThresholds = {
            temp: 30,
            hum: 70,
            gas: 2000,
            light: 500
        };

        // Initialisation au chargement
        document.addEventListener('DOMContentLoaded', function() {
            initCharts();
            connectMQTT();
            addLog('info', 'Initialisation du système...');
        });

        // Connexion MQTT
        function connectMQTT() {
            addLog('info', 'Connexion au broker MQTT...');
            
            // Générer un ID client unique avec timestamp et valeur aléatoire crypto
            const timestamp = Date.now().toString(36);
            const randomPart = Array.from(crypto.getRandomValues(new Uint8Array(8)))
                .map(b => b.toString(16).padStart(2, '0'))
                .join('');
            const clientId = 'sims-web-' + timestamp + '-' + randomPart.substring(0, 8);
            
            mqttClient = mqtt.connect('ws://broker.hivemq.com:8000/mqtt', {
                clientId: clientId,
                clean: true,
                reconnectPeriod: 5000
            });

            mqttClient.on('connect', function() {
                addLog('info', 'Connecté au broker MQTT');
                document.getElementById('mqttStatus').innerHTML = '<i class="fas fa-check-circle"></i> Connecté';
                document.getElementById('mqttStatus').className = 'status-badge status-connected';
                
                // Souscription aux topics
                mqttClient.subscribe('sims/sensors/data', function(err) {
                    if (!err) addLog('info', 'Souscription au topic: sims/sensors/data');
                });
                mqttClient.subscribe('sims/status', function(err) {
                    if (!err) addLog('info', 'Souscription au topic: sims/status');
                });
            });

            mqttClient.on('message', function(topic, message) {
                handleMQTTMessage(topic, message.toString());
            });

            mqttClient.on('error', function(error) {
                addLog('error', 'Erreur MQTT: ' + error.message);
            });

            mqttClient.on('offline', function() {
                addLog('warning', 'Déconnecté du broker MQTT');
                document.getElementById('mqttStatus').innerHTML = '<i class="fas fa-times-circle"></i> Déconnecté';
                document.getElementById('mqttStatus').className = 'status-badge status-disconnected';
            });
        }

        // Traitement des messages MQTT
        function handleMQTTMessage(topic, message) {
            try {
                if (topic === 'sims/sensors/data') {
                    const data = JSON.parse(message);
                    updateSensorDisplay(data);
                    updateCharts(data);
                    
                    // Mise à jour de l'heure
                    const now = new Date();
                    document.getElementById('lastUpdate').textContent = 
                        'Dernière mise à jour: ' + now.toLocaleTimeString();
                }
                else if (topic === 'sims/status') {
                    const status = JSON.parse(message);
                    addLog('info', 'Statut système reçu: ' + JSON.stringify(status));
                }
            } catch (e) {
                addLog('error', 'Erreur parsing JSON: ' + e.message);
            }
        }

        // Mise à jour de l'affichage des capteurs
        function updateSensorDisplay(data) {
            // Température
            document.getElementById('tempValue').textContent = data.temperature.toFixed(1);
            const tempCard = document.getElementById('tempCard');
            if (data.temperature > currentThresholds.temp) {
                tempCard.classList.add('alarm');
            } else {
                tempCard.classList.remove('alarm');
            }
            
            // Humidité
            document.getElementById('humValue').textContent = data.humidity.toFixed(1);
            const humCard = document.getElementById('humCard');
            if (data.humidity > currentThresholds.hum) {
                humCard.classList.add('alarm');
            } else {
                humCard.classList.remove('alarm');
            }
            
            // Gaz
            document.getElementById('gasValue').textContent = data.gas;
            const gasCard = document.getElementById('gasCard');
            if (data.gas > currentThresholds.gas) {
                gasCard.classList.add('alarm');
            } else {
                gasCard.classList.remove('alarm');
            }
            
            // Luminosité
            document.getElementById('lightValue').textContent = data.light;
            const lightCard = document.getElementById('lightCard');
            if (data.light < currentThresholds.light) {
                lightCard.classList.add('alarm');
            } else {
                lightCard.classList.remove('alarm');
            }
            
            // État des actionneurs
            updateRelayButton(data.relay);
            updateAlarmButton(data.alarm);
            
            // Log si alarme
            if (data.alarm) {
                addLog('warning', 'ALARME ACTIVE!');
            }
        }

        // Mise à jour des graphiques
        function updateCharts(data) {
            const now = new Date().toLocaleTimeString();
            
            // Ajouter les nouvelles données
            sensorData.timestamps.push(now);
            sensorData.temperature.push(data.temperature);
            sensorData.humidity.push(data.humidity);
            sensorData.gas.push(data.gas);
            sensorData.light.push(data.light);
            
            // Limiter le nombre de points
            if (sensorData.timestamps.length > MAX_DATA_POINTS) {
                sensorData.timestamps.shift();
                sensorData.temperature.shift();
                sensorData.humidity.shift();
                sensorData.gas.shift();
                sensorData.light.shift();
            }
            
            // Mettre à jour les graphiques
            tempHumChart.data.labels = sensorData.timestamps;
            tempHumChart.data.datasets[0].data = sensorData.temperature;
            tempHumChart.data.datasets[1].data = sensorData.humidity;
            tempHumChart.update('none');
            
            gasLightChart.data.labels = sensorData.timestamps;
            gasLightChart.data.datasets[0].data = sensorData.gas;
            gasLightChart.data.datasets[1].data = sensorData.light;
            gasLightChart.update('none');
        }

        // Initialisation des graphiques
        function initCharts() {
            // Graphique Température/Humidité
            const ctx1 = document.getElementById('tempHumChart').getContext('2d');
            tempHumChart = new Chart(ctx1, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Température (°C)',
                        data: [],
                        borderColor: '#e74c3c',
                        backgroundColor: 'rgba(231, 76, 60, 0.1)',
                        tension: 0.4
                    }, {
                        label: 'Humidité (%)',
                        data: [],
                        borderColor: '#3498db',
                        backgroundColor: 'rgba(52, 152, 219, 0.1)',
                        tension: 0.4
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: true,
                    interaction: {
                        intersect: false,
                        mode: 'index'
                    },
                    plugins: {
                        legend: {
                            display: true,
                            position: 'top'
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true
                        }
                    }
                }
            });
            
            // Graphique Gaz/Luminosité
            const ctx2 = document.getElementById('gasLightChart').getContext('2d');
            gasLightChart = new Chart(ctx2, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Gaz',
                        data: [],
                        borderColor: '#f39c12',
                        backgroundColor: 'rgba(243, 156, 18, 0.1)',
                        tension: 0.4,
                        yAxisID: 'y'
                    }, {
                        label: 'Luminosité',
                        data: [],
                        borderColor: '#f1c40f',
                        backgroundColor: 'rgba(241, 196, 15, 0.1)',
                        tension: 0.4,
                        yAxisID: 'y1'
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: true,
                    interaction: {
                        intersect: false,
                        mode: 'index'
                    },
                    plugins: {
                        legend: {
                            display: true,
                            position: 'top'
                        }
                    },
                    scales: {
                        y: {
                            type: 'linear',
                            display: true,
                            position: 'left',
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: 'Gaz'
                            }
                        },
                        y1: {
                            type: 'linear',
                            display: true,
                            position: 'right',
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: 'Luminosité'
                            },
                            grid: {
                                drawOnChartArea: false
                            }
                        }
                    }
                }
            });
        }

        // Contrôle du relais
        function toggleRelay() {
            const btn = document.getElementById('btnRelay');
            const isActive = btn.classList.contains('active');
            const newState = !isActive ? 'ON' : 'OFF';
            
            const payload = JSON.stringify({ state: newState });
            mqttClient.publish('sims/control/relay', payload);
            addLog('info', 'Commande relais envoyée: ' + newState);
        }

        // Contrôle de l'alarme
        function toggleAlarm() {
            const btn = document.getElementById('btnAlarm');
            const isActive = btn.classList.contains('active');
            const newState = !isActive ? 'ON' : 'OFF';
            
            const payload = JSON.stringify({ state: newState });
            mqttClient.publish('sims/control/alarm', payload);
            addLog('info', 'Commande alarme envoyée: ' + newState);
        }

        // Mise à jour des boutons
        function updateRelayButton(state) {
            const btn = document.getElementById('btnRelay');
            if (state) {
                btn.classList.remove('inactive');
                btn.classList.add('active');
                btn.innerHTML = '<i class="fas fa-fan fa-spin"></i> Relais: ON';
            } else {
                btn.classList.remove('active');
                btn.classList.add('inactive');
                btn.innerHTML = '<i class="fas fa-fan"></i> Relais: OFF';
            }
        }

        function updateAlarmButton(state) {
            const btn = document.getElementById('btnAlarm');
            if (state) {
                btn.classList.remove('inactive');
                btn.classList.add('active');
                btn.innerHTML = '<i class="fas fa-bell"></i> Alarme: ON';
            } else {
                btn.classList.remove('active');
                btn.classList.add('inactive');
                btn.innerHTML = '<i class="fas fa-bell"></i> Alarme: OFF';
            }
        }

        // Mise à jour des seuils
        function updateThresholds(event) {
            event.preventDefault();
            
            const temp = parseFloat(document.getElementById('inputTemp').value);
            const hum = parseFloat(document.getElementById('inputHum').value);
            const gas = parseInt(document.getElementById('inputGas').value);
            const light = parseInt(document.getElementById('inputLight').value);
            
            currentThresholds = { temp, hum, gas, light };
            
            const payload = JSON.stringify({ temp, hum, gas, light });
            mqttClient.publish('sims/control/thresholds', payload);
            
            // Mise à jour de l'affichage
            document.getElementById('tempThreshold').textContent = temp;
            document.getElementById('humThreshold').textContent = hum;
            document.getElementById('gasThreshold').textContent = gas;
            document.getElementById('lightThreshold').textContent = light;
            
            addLog('info', 'Seuils mis à jour: T=' + temp + '°C, H=' + hum + '%, G=' + gas + ', L=' + light);
        }

        // Rafraîchir les données
        function refreshData() {
            addLog('info', 'Demande de rafraîchissement...');
            mqttClient.publish('sims/control/refresh', '1');
        }

        // Ajouter un log
        function addLog(type, message) {
            const console = document.getElementById('logConsole');
            const timestamp = new Date().toLocaleTimeString();
            const entry = document.createElement('div');
            entry.className = 'log-entry ' + type;
            
            let prefix = '[INFO]';
            if (type === 'warning') prefix = '[WARN]';
            if (type === 'error') prefix = '[ERROR]';
            
            entry.textContent = `[${timestamp}] ${prefix} ${message}`;
            console.appendChild(entry);
            console.scrollTop = console.scrollHeight;
            
            // Limiter le nombre de logs
            while (console.children.length > 100) {
                console.removeChild(console.firstChild);
            }
        }

        // Initialiser les seuils par défaut dans le formulaire
        window.addEventListener('load', function() {
            document.getElementById('inputTemp').value = currentThresholds.temp;
            document.getElementById('inputHum').value = currentThresholds.hum;
            document.getElementById('inputGas').value = currentThresholds.gas;
            document.getElementById('inputLight').value = currentThresholds.light;
            
            document.getElementById('tempThreshold').textContent = currentThresholds.temp;
            document.getElementById('humThreshold').textContent = currentThresholds.hum;
            document.getElementById('gasThreshold').textContent = currentThresholds.gas;
            document.getElementById('lightThreshold').textContent = currentThresholds.light;
        });
    </script>
</body>
</html>

)rawliteral";
  
  webServer.send(200, "text/html", html);
}

void handleStatus() {
  StaticJsonDocument<256> doc;
  doc["status"] = "online";
  doc["wifi"] = WiFi.status() == WL_CONNECTED;
  doc["mqtt"] = mqttClient.connected();
  doc["alarm"] = alarmActive;
  doc["relay"] = digitalRead(RELAY_PIN);
  doc["uptime"] = millis() / 1000;
  doc["ip"] = WiFi.localIP().toString();
  
  String output;
  serializeJson(doc, output);
  
  webServer.send(200, "application/json", output);
}

void handleSensors() {
  StaticJsonDocument<256> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["gas"] = gasLevel;
  doc["light"] = lightLevel;
  doc["alarm"] = alarmActive;
  doc["relay"] = digitalRead(RELAY_PIN);
  doc["timestamp"] = millis() / 1000;
  
  String output;
  serializeJson(doc, output);
  
  webServer.send(200, "application/json", output);
}

void handleNotFound() {
  String message = "404 - Page non trouvée\n\n";
  message += "URI: " + webServer.uri() + "\n";
  message += "Méthode: " + String((webServer.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  
  webServer.send(404, "text/plain", message);
}
