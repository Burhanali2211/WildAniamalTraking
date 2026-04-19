#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DEVICE_NAME "Billi_Meow"
#define BROADCAST_INTERVAL 3000  // Send every 3 seconds

// WiFi Credentials
const char* ssid = "Faheem";
const char* password = "12345678";

// Server address - will use this to send data
const char* serverIP = "10.156.107.82";  // Server IP (update after server starts)
const int serverPort = 8080;
String serverUrl = "";

unsigned long lastSend = 0;
int sendCount = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n=== SKICC Wildlife Tracker (Animal) ===");
  Serial.println("Initializing...");

  // Connect to WiFi
  Serial.print("📡 Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected");
    Serial.print("🌐 Animal IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    return;
  }

  // Build server URL
  serverUrl = "http://";
  serverUrl += serverIP;
  serverUrl += ":";
  serverUrl += serverPort;
  serverUrl += "/api/location";

  Serial.print("📍 Server URL: ");
  Serial.println(serverUrl);
  Serial.println("✅ Ready to send location data");
  Serial.println("");
}

void sendLocationToServer() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi disconnected!");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  // Create JSON data
  String jsonData = "{\"animal\":\"" + String(DEVICE_NAME) + "\",\"zone\":\"SKICC_MainHall\"}";

  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.print("s] ");

  if (http.begin(client, serverUrl)) {
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonData);

    Serial.print("📡 Sent: ");
    Serial.print(DEVICE_NAME);
    Serial.print(" | Response: ");
    Serial.print(httpResponseCode);

    if (httpResponseCode == 200) {
      Serial.println(" ✅");
      sendCount++;
    } else {
      Serial.println(" ❌");
    }

    http.end();
  } else {
    Serial.println("❌ Failed to connect to server");
  }
}

void loop() {
  unsigned long now = millis();

  if (now - lastSend >= BROADCAST_INTERVAL) {
    sendLocationToServer();
    lastSend = now;
  }

  delay(10);
}