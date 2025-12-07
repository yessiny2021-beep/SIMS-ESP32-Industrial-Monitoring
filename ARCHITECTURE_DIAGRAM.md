# Architecture S.I.M.S. v2.0 - Web & MQTT

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         SMART INDUSTRIAL MONITORING SYSTEM                  │
│                                  (S.I.M.S. v2.0)                            │
└─────────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────────────────┐
│                              LAYER 1: SENSORS                             │
├─────────────┬─────────────┬─────────────┬──────────────────────────────────┤
│   DHT22     │    MQ2      │    LDR      │                                  │
│ (Temp/Hum)  │   (Gaz)     │ (Lumière)   │                                  │
│  GPIO 15    │  GPIO 34    │  GPIO 35    │                                  │
└──────┬──────┴──────┬──────┴──────┬──────┘                                  │
       │             │             │                                          │
       └─────────────┴─────────────┘                                          │
                     │                                                         │
┌────────────────────▼──────────────────────────────────────────────────────┐
│                         LAYER 2: ESP32 CONTROLLER                         │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌────────────────────────────────────────────────────────────────────┐   │
│  │                    MAIN PROGRAM (sketch.ino)                       │   │
│  ├────────────────────────────────────────────────────────────────────┤   │
│  │                                                                     │   │
│  │  • Lecture capteurs (2s)                                           │   │
│  │  • Vérification seuils                                             │   │
│  │  • Gestion alarmes                                                 │   │
│  │  • Sauvegarde EEPROM                                               │   │
│  │  • Affichage LCD                                                   │   │
│  │                                                                     │   │
│  └─────────────────┬──────────────────────────────────────────────────┘   │
│                    │                                                        │
│  ┌─────────────────▼──────────────────┬──────────────────────────────┐   │
│  │    WiFi Manager                    │   MQTT Client                │   │
│  │  • SSID: Wokwi-GUEST               │  • Broker: hivemq.com       │   │
│  │  • Auto-connect                    │  • Port: 1883               │   │
│  │  • IP assignment                   │  • Client ID: MAC-based     │   │
│  └────────────┬───────────────────────┴────────────┬─────────────────┘   │
│               │                                     │                      │
│  ┌────────────▼────────────────────┐  ┌────────────▼─────────────────┐   │
│  │      Web Server (Port 80)       │  │     MQTT Topics              │   │
│  ├─────────────────────────────────┤  ├──────────────────────────────┤   │
│  │ Routes:                         │  │ Publish:                     │   │
│  │  • / (Home)                     │  │  • sims/sensors/data (5s)   │   │
│  │  • /api/sensors (JSON)          │  │  • sims/status              │   │
│  │  • /api/status (JSON)           │  │                             │   │
│  │                                 │  │ Subscribe:                  │   │
│  │ Features:                       │  │  • sims/control/relay       │   │
│  │  • HTML serving                 │  │  • sims/control/alarm       │   │
│  │  • JSON API                     │  │  • sims/control/thresholds  │   │
│  │  • CORS enabled                 │  │  • sims/control/refresh     │   │
│  └─────────────┬───────────────────┘  └────────────┬─────────────────┘   │
│                │                                     │                      │
└────────────────┼─────────────────────────────────────┼──────────────────────┘
                 │                                     │
                 │  HTTP/REST                          │  MQTT Protocol
                 │                                     │
┌────────────────▼─────────────────────────────────────▼──────────────────┐
│                         LAYER 3: CLOUD BROKER                            │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │              MQTT Broker (broker.hivemq.com)                     │   │
│  ├──────────────────────────────────────────────────────────────────┤   │
│  │  • Public broker (free)                                          │   │
│  │  • Port 1883 (MQTT)                                              │   │
│  │  • Port 8000 (WebSocket)                                         │   │
│  │  • No authentication required                                    │   │
│  │  • Message routing                                               │   │
│  │  • QoS support                                                   │   │
│  └──────────────────────┬───────────────────────────────────────────┘   │
│                         │                                                 │
└─────────────────────────┼─────────────────────────────────────────────────┘
                          │  WebSocket (Port 8000)
                          │
┌─────────────────────────▼─────────────────────────────────────────────────┐
│                       LAYER 4: WEB INTERFACE                              │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌────────────────────────────────────────────────────────────────────┐   │
│  │               Web Dashboard (data/index.html)                      │   │
│  ├────────────────────────────────────────────────────────────────────┤   │
│  │                                                                     │   │
│  │  ┌──────────────────────────────────────────────────────────────┐ │   │
│  │  │                    HEADER                                     │ │   │
│  │  │  • Logo S.I.M.S.                                             │ │   │
│  │  │  • MQTT Status Badge (Connected/Disconnected)                │ │   │
│  │  │  • Last Update Time                                          │ │   │
│  │  └──────────────────────────────────────────────────────────────┘ │   │
│  │                                                                     │   │
│  │  ┌───────────┬───────────┬───────────┬───────────┐                │   │
│  │  │TEMPERATURE│ HUMIDITY  │   GAS     │   LIGHT   │                │   │
│  │  │           │           │           │           │                │   │
│  │  │  25.5°C   │   60.0%   │   1500    │   2000    │                │   │
│  │  │  🌡️       │   💧      │   ☁️     │   💡      │                │   │
│  │  │ Max: 30°C │ Max: 70%  │ Max: 2000 │ Min: 500  │                │   │
│  │  └───────────┴───────────┴───────────┴───────────┘                │   │
│  │                                                                     │   │
│  │  ┌───────────────────────────┬───────────────────────────┐         │   │
│  │  │   Chart: Temp & Humidity  │   Chart: Gas & Light      │         │   │
│  │  │  ╭─────────────────────╮  │  ╭─────────────────────╮  │         │   │
│  │  │  │ [Line Graph]        │  │  │ [Dual-axis Graph]   │  │         │   │
│  │  │  │ 20 points history   │  │  │ 20 points history   │  │         │   │
│  │  │  │ Auto-update (5s)    │  │  │ Auto-update (5s)    │  │         │   │
│  │  │  ╰─────────────────────╯  │  ╰─────────────────────╯  │         │   │
│  │  └───────────────────────────┴───────────────────────────┘         │   │
│  │                                                                     │   │
│  │  ┌──────────────────┬──────────────────────────────────────────┐   │   │
│  │  │   CONTROLS       │   THRESHOLD CONFIGURATION                │   │   │
│  │  ├──────────────────┤                                          │   │   │
│  │  │ [Relay: OFF]     │  Temperature Max: [___] °C               │   │   │
│  │  │ [Alarm: OFF]     │  Humidity Max:    [___] %                │   │   │
│  │  │ [🔄 Refresh]     │  Gas Max:         [___] (0-4095)         │   │   │
│  │  │                  │  Light Min:       [___] (0-4095)         │   │   │
│  │  │                  │  [💾 Save Thresholds]                    │   │   │
│  │  └──────────────────┴──────────────────────────────────────────┘   │   │
│  │                                                                     │   │
│  │  ┌──────────────────────────────────────────────────────────────┐ │   │
│  │  │                    EVENT CONSOLE                             │ │   │
│  │  ├──────────────────────────────────────────────────────────────┤ │   │
│  │  │ [16:30:15] [INFO] Connected to MQTT broker                  │ │   │
│  │  │ [16:30:20] [INFO] Subscribed to: sims/sensors/data          │ │   │
│  │  │ [16:30:25] [INFO] Sensor data received                      │ │   │
│  │  │ [16:30:30] [WARN] ALARM ACTIVE!                             │ │   │
│  │  │ [16:30:35] [INFO] Relay command sent: ON                    │ │   │
│  │  └──────────────────────────────────────────────────────────────┘ │   │
│  │                                                                     │   │
│  └────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Technologies:                                                              │
│  • Bootstrap 5.3.0 (Responsive CSS)                                        │
│  • Chart.js 4.3.0 (Interactive Charts)                                     │
│  • MQTT.js 5.0.0 (WebSocket Client)                                        │
│  • Font Awesome 6.4.0 (Icons)                                              │
│  • Vanilla JavaScript (No frameworks)                                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                          LAYER 5: ACTUATORS                                 │
├─────────────┬─────────────┬─────────────┬──────────────┬───────────────────┤
│   BUZZER    │  LED RED    │   RELAY     │  LED BLUE    │   LCD 16x2        │
│  (Alarm)    │  (Alarm)    │ (Vent/Ext)  │  (Status)    │  (Display)        │
│  GPIO 25    │  GPIO 26    │  GPIO 27    │  GPIO 2      │  I2C (21,22)      │
└─────────────┴─────────────┴─────────────┴──────────────┴───────────────────┘

═══════════════════════════════════════════════════════════════════════════════

DATA FLOW:

1. SENSOR READING (Every 2s)
   Sensors → ESP32 → Local Processing → Threshold Check

2. MQTT PUBLICATION (Every 5s)
   ESP32 → JSON Formatting → MQTT Publish → Broker → Web Interface

3. WEB CONTROL (On user action)
   Web Interface → MQTT Message → Broker → ESP32 → Actuator Action

4. API REST (On HTTP request)
   Browser → HTTP GET → ESP32 → JSON Response

═══════════════════════════════════════════════════════════════════════════════

MESSAGE FORMATS:

📤 MQTT Publish (sims/sensors/data):
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

📥 MQTT Subscribe (sims/control/relay):
{"state": "ON"}  or  {"state": "OFF"}

📥 MQTT Subscribe (sims/control/thresholds):
{
  "temp": 35,
  "hum": 75,
  "gas": 2500,
  "light": 600
}

═══════════════════════════════════════════════════════════════════════════════

SECURITY FEATURES:

✓ Unique MQTT Client IDs (MAC-based for ESP32, crypto-random for web)
✓ Input validation (NULL checks, range validation)
✓ Error handling with descriptive messages
✓ No hardcoded secrets
✓ CORS properly configured
✓ JSON buffer sized appropriately (256 bytes)

═══════════════════════════════════════════════════════════════════════════════

PERFORMANCE:

• Sensor reading:     2 seconds interval
• MQTT publishing:    5 seconds interval
• Display update:     500ms interval
• Chart update:       5 seconds (20 points max)
• Auto-reconnect:     5 seconds on disconnect
• Web response time:  < 100ms

═══════════════════════════════════════════════════════════════════════════════
