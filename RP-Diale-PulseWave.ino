#include <WiFi.h>
#include <HTTPClient.h>
#include <PulseSensorPlayground.h>
#include <DFRobot_MLX90614.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
const char* ssid = "VodafoneMobileWiFi-CECE77";
const char* password = "4830970720";

// ThingSpeak settings
const char* thingSpeakApiKey = "QM6FSNLXACU35YQE";
const unsigned long thingSpeakPostDelay = 15000; // 15 sec
unsigned long lastThingSpeakPost = 0;

// Pulse Sensor Setup
const int PulseWire = 34;       // PulseSensor connected to pin 34
int Threshold = 2000;           // Threshold for detection

PulseSensorPlayground pulseSensor;
DFRobot_MLX90614_I2C mlxSensor;

// LCD Setup 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Web server
WiFiServer server(80);

// Variables
int myBPM = 0;
unsigned long lastBeatTime = 0;
bool beatDetected = false;
int signalStrength = 0;
float ambientTemp = 0;
float objectTemp = 0;
int rawPulseValue = 0;

// Timers
unsigned long lastTempRead = 0;
unsigned long lastDisplay = 0;
unsigned long lastLCD = 0;

//  HEALTH STATE FUNCTIONS
String getPulseState(int bpm) {
  if (bpm < 60) {
    return "Low Pulse (Bradycardia) - Possible causes: fatigue, heart issues, or medications.";
  } else if (bpm > 100) {
    return "High Pulse (Tachycardia) - Possible causes: exercise, stress, fever, or dehydration.";
  } else {
    return "Normal Pulse - Body is fine.";
  }
}

String getTempState(float temp) {
  if (temp < 36.0) {
    return "Low Temperature (Hypothermia risk) - Possible causes: cold exposure or circulation issues.";
  } else if (temp > 37.5) {
    return "High Temperature (Fever) - Possible causes: infection, overheating, or inflammation.";
  } else {
    return "Normal Temperature - Body is fine.";
  }
}

// SETUP
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing ESP32 Health Monitor...");

  // Init I2C + LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("IoT Health Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Configure Pulse Sensor
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.setSerial(Serial);

  if (pulseSensor.begin()) {
    Serial.println("PulseSensor initialized!");
  } else {
    Serial.println("PulseSensor failed!");
  }

  // Init MLX90614 Temp Sensor
  if (NO_ERR != mlxSensor.begin()) {
    Serial.println("MLX90614 failed, check wiring.");
  } else {
    Serial.println("MLX90614 initialized!");
  }

  // Wi-Fi connect
  connectToWiFi();

  // Start web server
  server.begin();
  Serial.println("Web server started");
  lcd.clear();
}

// MAIN LOOP
void loop() {
  unsigned long currentMillis = millis();

  // Reconnect WiFi if dropped
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Read raw pulse
  rawPulseValue = analogRead(PulseWire);
  signalStrength = rawPulseValue;

  if (pulseSensor.sawStartOfBeat()) {
    myBPM = pulseSensor.getBeatsPerMinute();
    lastBeatTime = currentMillis;
    beatDetected = true;
    Serial.println("♥ HeartBeat detected!");
  }

  // Read temperature every 1 sec
  if (currentMillis - lastTempRead >= 1000) {
    ambientTemp = mlxSensor.getAmbientTempCelsius();
    objectTemp = mlxSensor.getObjectTempCelsius();
    lastTempRead = currentMillis;
  }

  // Print to Serial every 1 sec
  if (currentMillis - lastDisplay >= 1000) {
    Serial.println("SENSOR READINGS");
    Serial.print("BPM: "); Serial.println(myBPM);
    Serial.print("Signal: "); Serial.println(signalStrength);
    Serial.print("Object Temp: "); Serial.print(objectTemp); Serial.println("°C");
    Serial.print("Ambient Temp: "); Serial.print(ambientTemp); Serial.println("°C");
    Serial.println("Pulse Status: " + getPulseState(myBPM));
    Serial.println("Temperature Status: " + getTempState(objectTemp));
    lastDisplay = currentMillis;
  }

  // Update LCD every 1 sec
  if (currentMillis - lastLCD >= 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (beatDetected && (currentMillis - lastBeatTime < 5000)) {
      lcd.print("Pulse: " + String(myBPM) + " bpm");
    } else {
      lcd.print("Pulse: ---");
    }
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(objectTemp, 1) + " C");
    lastLCD = currentMillis;
  }

  // Send to ThingSpeak every 15 sec
  if (currentMillis - lastThingSpeakPost > thingSpeakPostDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      sendToThingSpeak();
    }
    lastThingSpeakPost = currentMillis;
  }

  // Web client
  WiFiClient client = server.available();
  if (client) {
    handleClientRequest(client);
  }
}

// FUNCTIONS
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi connect failed.");
  }
}

void sendToThingSpeak() {
  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key=" + String(thingSpeakApiKey) +
               "&field1=" + String(myBPM) +
               "&field2=" + String(signalStrength) +
               "&field3=" + String(objectTemp) +
               "&field4=" + String(ambientTemp);
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("Data sent to ThingSpeak.");
  } else {
    Serial.println("ThingSpeak send failed.");
  }
  http.end();
}

// WEB DASHBOARD
void handleClientRequest(WiFiClient client) {
  String currentLine = "";
  unsigned long timeout = millis() + 250;

  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        if (currentLine.length() == 0) {
          // Send HTTP response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // Web page content
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<meta http-equiv=\"refresh\" content=\"2\">"); // faster refresh
          client.println("<link rel=\"icon\" href=\"data:,\">");
          client.println("<style>");
          client.println("body { text-align: center; font-family: Arial, sans-serif; background: linear-gradient(to bottom, #1a1a2e, #16213e); color: white; margin: 0; padding: 20px; }");
          client.println(".container { max-width: 600px; margin: 0 auto; background: rgba(255, 255, 255, 0.1); border-radius: 15px; padding: 20px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); }");
          client.println(".heart { color: #e94560; font-size: 3em; animation: pulse 1.5s infinite; }");
          client.println("@keyframes pulse { 0% { transform: scale(1); } 50% { transform: scale(1.1); } 100% { transform: scale(1); } }");
          client.println(".data { font-size: 1.8em; margin: 20px; padding: 15px; background: rgba(255, 255, 255, 0.15); border-radius: 10px; }");
          client.println(".temp-data { font-size: 1.4em; margin: 15px; padding: 10px; background: rgba(255, 255, 255, 0.1); border-radius: 8px; }");
          client.println(".signal { height: 20px; background: #4cc9f0; border-radius: 10px; margin: 10px 0; }");
          client.println("h1 { color: #4cc9f0; }");
          client.println(".status { margin-top: 20px; font-size: 0.9em; color: #a5b1c2; }");
          client.println("</style></head>");
          client.println("<body><div class='container'>");
          client.println("<h1>ESP32 Health Monitor</h1>");

          if (beatDetected && (millis() - lastBeatTime < 5000)) {
            client.println("<div class=\"heart\">♥</div>");
            client.println("<div class=\"data\">BPM: " + String(myBPM) + "</div>");
            client.println("<div>Signal Strength: " + String(signalStrength) + "</div>");
            client.println("<div class=\"signal\" style=\"width: " + String(map(signalStrength, 0, 4095, 0, 100)) + "%;\"></div>");
            client.println("<div>Last beat: " + String((millis() - lastBeatTime)/1000) + " seconds ago</div>");
          } else {
            client.println("<div class=\"data\">Place your finger on the sensor</div>");
            client.println("<div>Signal Strength: " + String(signalStrength) + "</div>");
            client.println("<div class=\"signal\" style=\"width: " + String(map(signalStrength, 0, 4095, 0, 100)) + "%;\"></div>");
            beatDetected = false;
          }

          // Temperature data
          client.println("<div class=\"temp-data\">");
          client.println("Object Temperature: " + String(objectTemp, 1) + "°C<br>");
          client.println("Ambient Temperature: " + String(ambientTemp, 1) + "°C");
          client.println("</div>");

          // Show health states
          client.println("<div class=\"status\">");
          client.println("Pulse Status: " + getPulseState(myBPM) + "<br>");
          client.println("Temperature Status: " + getTempState(objectTemp));
          client.println("</div>");

          // WiFi & ThingSpeak
          client.println("<div class=\"status\">");
          client.println("ThingSpeak: " + String((WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected"));
          client.println("<br>Next update in: " + String((thingSpeakPostDelay - (millis() - lastThingSpeakPost)) / 1000) + "s");
          client.println("</div>");

          client.println("</div></body></html>");
          break;
        } else {
          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }
  client.stop();
}
