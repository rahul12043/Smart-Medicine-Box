#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

SoftwareSerial arduinoSerial(D5, D6); // RX=D5, TX=D6

const char* ssid = "your_wifi_name";
const char* password = "your_password";
const char* serverUrl = "http://192.168.111.68:5000";

void setup() {
  Serial.begin(115200);
  arduinoSerial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  // 1. Forward Arduino data to server
  if (arduinoSerial.available()) {
    String data = arduinoSerial.readStringUntil('\n');
    data.trim();
    if (data.length() > 30) { // Basic validation
      sendToServer("/log_data", data);
    }
  }

  // 2. Periodically check for server commands every 5 seconds
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    String command = getServerCommand();
    if (command.length() > 0) {
      arduinoSerial.println(command); // Forward to Arduino
    }
    lastCheck = millis();
  }
}

// 3. Fetch command from the server
String getServerCommand() {
  if (WiFi.status() != WL_CONNECTED) return "";

  WiFiClient client;
  HTTPClient http;
  String url = String(serverUrl) + "/get_command";
  http.begin(client, url);  // ✅ Use new API

  int httpCode = http.GET();
  String payload = "";

  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
  }

  http.end();
  return payload;
}

// 4. Send data to server
void sendToServer(String endpoint, String payload) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient client;
  HTTPClient http;
  String url = serverUrl + endpoint;
  http.begin(client, url);  // ✅ Use new API
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);
  Serial.print("POST Status: ");
  Serial.println(httpCode);

  http.end();
}
