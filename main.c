#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

const char* ssid = "Trong Hao 2.4G";
const char* password = "nguyentronghao";
ESP8266WebServer server(80);  // Web server running on port 80

const int relay = 5;           // D1 pin for the relay
const int lightsensor = A0;    // Analog pin for light sensor
const int lightThreshold = 700; // Threshold value for dark/light detection

bool manualControl = false;
bool sensorActive = true;
unsigned long lastSensorCheck = 0;
const unsigned long sensorCheckInterval = 1000; // Check sensor every 1 second

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  Serial.println("ESP IP:");
  Serial.println(WiFi.localIP());

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH); // Initially turn off the relay

  // Setup HTTP routes
  server.on("/on", handleLightOn);
  server.on("/off", handleLightOff);
  server.on("/sensoron", handleSensorOn);
  server.on("/sensoroff", handleSensorOff);

  server.begin();
  Serial.println("HTTP server started");
}

void handleLightOn() {
  digitalWrite(relay, LOW); // Turn on the relay
  server.send(200, "text/plain", "Light ON");
  Serial.println("Light ON");
}

void handleLightOff() {
  digitalWrite(relay, HIGH); // Turn off the relay
  server.send(200, "text/plain", "Light OFF");
  Serial.println("Light OFF");
}

void handleSensorOn() {
  sensorActive = true;
  manualControl = false;
  server.send(200, "text/plain", "Sensor ON");
  Serial.println("Sensor ON");
}

void handleSensorOff() {
  sensorActive = false;
  manualControl = true;
  server.send(200, "text/plain", "Sensor OFF");
  Serial.println("Sensor OFF");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check the sensor only if it's active and at the specified interval
  if (sensorActive && (currentMillis - lastSensorCheck >= sensorCheckInterval)) {
    lastSensorCheck = currentMillis;
    int sensorValue = analogRead(lightsensor);

    if (sensorValue > lightThreshold) {
      digitalWrite(relay, LOW); // Turn on the light
    } else {
      digitalWrite(relay, HIGH); // Turn off the light
    }
    Serial.println("Sensor value: " + String(sensorValue));
  }

  // Handle incoming client requests
  server.handleClient();
}
