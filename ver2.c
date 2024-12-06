#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP8266_AP"; // Tên mạng Wi-Fi
const char* password = "12345678"; // Mật khẩu mạng Wi-Fi

ESP8266WebServer server(80);  // Web server chạy trên cổng 80

const int relay = 5;           // D1 pin cho relay
const int lightsensor = A0;    // Pin analog cho cảm biến ánh sáng
const int lightThreshold = 700; // Ngưỡng ánh sáng để phát hiện tối/sáng

bool manualControl = false;
bool sensorActive = true;
unsigned long lastSensorCheck = 0;
const unsigned long sensorCheckInterval = 1000; // Kiểm tra cảm biến mỗi 1 giây

void setup() {
  Serial.begin(115200);

  // Cấu hình Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP()); // In địa chỉ IP của Access Point

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH); // Ban đầu tắt relay

  // Cấu hình các route HTTP
  server.on("/on", handleLightOn);
  server.on("/off", handleLightOff);
  server.on("/sensoron", handleSensorOn);
  server.on("/sensoroff", handleSensorOff);

  server.begin();
  Serial.println("HTTP server started");
}

void handleLightOn() {
  digitalWrite(relay, LOW); // Bật relay
  server.send(200, "text/plain", "Light ON");
  Serial.println("Light ON");
}

void handleLightOff() {
  digitalWrite(relay, HIGH); // Tắt relay
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
  
  // Kiểm tra cảm biến nếu cảm biến được kích hoạt
  if (sensorActive && (currentMillis - lastSensorCheck >= sensorCheckInterval)) {
    lastSensorCheck = currentMillis;
    int sensorValue = analogRead(lightsensor);

    if (sensorValue > lightThreshold) {
      digitalWrite(relay, LOW); // Bật đèn
    } else {
      digitalWrite(relay, HIGH); // Tắt đèn
    }
    Serial.println("Sensor value: " + String(sensorValue));
  }

  // Xử lý các yêu cầu từ client
  server.handleClient();
}
