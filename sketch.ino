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
const char* mqttClientId = "sims-esp32";

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
  
  if (mqttClient.connect(mqttClientId)) {
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
    Serial.print(" Échec! Code: ");
    Serial.println(mqttClient.state());
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convertir le payload en String
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.printf("[MQTT] Message reçu sur %s: %s\n", topic, message.c_str());
  
  // Parser JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.println("[MQTT] Erreur parsing JSON");
    return;
  }
  
  // Traitement selon le topic
  if (strcmp(topic, topicControlRelay) == 0) {
    const char* state = doc["state"];
    if (strcmp(state, "ON") == 0) {
      digitalWrite(RELAY_PIN, HIGH);
      manualRelayControl = true;
      Serial.println("[MQTT] Relais activé manuellement");
    } else if (strcmp(state, "OFF") == 0) {
      digitalWrite(RELAY_PIN, LOW);
      manualRelayControl = false;
      Serial.println("[MQTT] Relais désactivé");
    }
  }
  else if (strcmp(topic, topicControlAlarm) == 0) {
    const char* state = doc["state"];
    if (strcmp(state, "ON") == 0) {
      manualAlarmControl = true;
      activateAlarm("ALARME MANUELLE");
      Serial.println("[MQTT] Alarme activée manuellement");
    } else if (strcmp(state, "OFF") == 0) {
      manualAlarmControl = false;
      deactivateAlarm();
      Serial.println("[MQTT] Alarme désactivée manuellement");
    }
  }
  else if (strcmp(topic, topicControlThresholds) == 0) {
    if (doc.containsKey("temp")) {
      thresholds.tempMax = doc["temp"];
    }
    if (doc.containsKey("hum")) {
      thresholds.humidityMax = doc["hum"];
    }
    if (doc.containsKey("gas")) {
      thresholds.gasMax = doc["gas"];
    }
    if (doc.containsKey("light")) {
      thresholds.lightMin = doc["light"];
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
  // Lire le fichier HTML depuis SPIFFS ou le retourner en tant que String
  // Pour Wokwi, on va embarquer le HTML directement dans le code
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>S.I.M.S. - Dashboard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        h1 {
            color: #1a2332;
            text-align: center;
            margin-bottom: 30px;
        }
        .info {
            background: #f0f0f0;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        .info h2 {
            margin-top: 0;
            color: #2c5f8d;
        }
        .btn {
            display: inline-block;
            padding: 10px 20px;
            background: #2c5f8d;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            margin: 10px 5px;
        }
        .btn:hover {
            background: #1a3d5c;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🏭 S.I.M.S. - Système de Surveillance Industrielle</h1>
        <div class="info">
            <h2>Bienvenue sur le serveur Web S.I.M.S.</h2>
            <p>Le système de surveillance industrielle est actif et connecté.</p>
            <p><strong>Adresse IP:</strong> )rawliteral" + WiFi.localIP().toString() + R"rawliteral(</p>
            <p><strong>État WiFi:</strong> Connecté</p>
            <p><strong>État MQTT:</strong> )rawliteral" + String(mqttClient.connected() ? "Connecté" : "Déconnecté") + R"rawliteral(</p>
        </div>
        <div class="info">
            <h2>📊 Données en Temps Réel</h2>
            <p><strong>Température:</strong> )rawliteral" + String(temperature, 1) + R"rawliteral( °C</p>
            <p><strong>Humidité:</strong> )rawliteral" + String(humidity, 1) + R"rawliteral( %</p>
            <p><strong>Niveau Gaz:</strong> )rawliteral" + String(gasLevel) + R"rawliteral(</p>
            <p><strong>Luminosité:</strong> )rawliteral" + String(lightLevel) + R"rawliteral(</p>
            <p><strong>Alarme:</strong> )rawliteral" + String(alarmActive ? "ACTIVE ⚠️" : "Inactive ✓") + R"rawliteral(</p>
        </div>
        <div class="info">
            <h2>🔗 Accès à l'Interface Complète</h2>
            <p>Pour accéder à l'interface web complète avec graphiques et contrôles MQTT, ouvrez le fichier <strong>data/index.html</strong> dans votre navigateur.</p>
            <p>Assurez-vous que votre navigateur peut se connecter au broker MQTT public: <strong>broker.hivemq.com:8000</strong></p>
            <a href="/api/sensors" class="btn">📡 API Capteurs (JSON)</a>
            <a href="/api/status" class="btn">📊 API Statut (JSON)</a>
        </div>
        <div class="info">
            <h2>📱 Topics MQTT</h2>
            <ul>
                <li><code>sims/sensors/data</code> - Données des capteurs (lecture)</li>
                <li><code>sims/control/relay</code> - Contrôle du relais (écriture)</li>
                <li><code>sims/control/alarm</code> - Contrôle de l'alarme (écriture)</li>
                <li><code>sims/control/thresholds</code> - Modification des seuils (écriture)</li>
                <li><code>sims/status</code> - Statut du système (lecture)</li>
            </ul>
        </div>
    </div>
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
