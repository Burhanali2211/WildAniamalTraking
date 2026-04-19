#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "Faheem";
const char* password = "12345678";

// Vercel API endpoint
const char* vercelAPI = "https://wild-aniamal-traking.vercel.app/api/update";
const int SEND_TIMEOUT = 30000;

// Web server on port 8080
WebServer server(8080);

#define BUZZER 5
#define SWITCH 0

unsigned long lastServerUpdate = 0;
const unsigned long MIN_UPDATE_INTERVAL = 1000;

// ================= SEND TO VERCEL API =================
bool sendToVercelAPI(String animal, String zone) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected - cannot send to API");
    return false;
  }

  unsigned long now = millis();
  if (now - lastServerUpdate < MIN_UPDATE_INTERVAL) {
    return false;  // Rate limiting
  }

  HTTPClient http;
  http.setConnectTimeout(SEND_TIMEOUT);
  
  if (!http.begin(vercelAPI)) {
    Serial.println("❌ HTTP begin failed");
    return false;
  }

  http.addHeader("Content-Type", "application/json");

  String json = "{\"animal\":\"" + animal + "\",\"zone\":\"" + zone + "\"}";

  Serial.print("📤 Forwarding to Vercel: ");
  Serial.println(json);

  int httpResponseCode = http.POST(json);
  bool success = (httpResponseCode == 200);

  Serial.print("📨 Vercel Response: ");
  Serial.print(httpResponseCode);
  Serial.print(" | Animal: ");
  Serial.print(animal);
  Serial.print(" | Zone: ");
  Serial.print(zone);
  Serial.println(success ? " ✅" : " ❌");

  http.end();
  
  if (success) {
    lastServerUpdate = now;
    
    // 🔊 Buzzer alert
    if (digitalRead(SWITCH) == HIGH) {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100);
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
    }
  }
  
  return success;
}

// ================= HTTP ENDPOINT TO RECEIVE FROM ANIMAL =================
void handleLocationUpdate() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String body = server.arg("plain");
  Serial.println("");
  Serial.print("🐾 Received from Animal: ");
  Serial.println(body);

  // Parse JSON (simple parsing)
  // Expected: {"animal":"Billi_Meow","zone":"SKICC_MainHall"}
  int animalStart = body.indexOf("\"animal\":\"") + 10;
  int animalEnd = body.indexOf("\"", animalStart);
  String animal = body.substring(animalStart, animalEnd);

  int zoneStart = body.indexOf("\"zone\":\"") + 8;
  int zoneEnd = body.indexOf("\"", zoneStart);
  String zone = body.substring(zoneStart, zoneEnd);

  Serial.print("✅ Parsed - Animal: ");
  Serial.print(animal);
  Serial.print(" | Zone: ");
  Serial.println(zone);

  // Forward to Vercel API
  bool success = sendToVercelAPI(animal, zone);

  // Send response back to animal
  if (success) {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(500, "application/json", "{\"status\":\"error\"}");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n=== SKICC Wildlife Server (Gateway) ===");
  Serial.println("Initializing...");

  pinMode(BUZZER, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
  digitalWrite(BUZZER, LOW);

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
    Serial.print("🌐 Server IP: ");
    Serial.println(WiFi.localIP());
    
    Serial.print("📶 WiFi Channel: ");
    Serial.println(WiFi.channel());
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    return;
  }

  // Start HTTP server
  server.on("/api/location", handleLocationUpdate);
  server.begin();

  Serial.print("🌐 HTTP Server started on port 8080");
  Serial.println("📍 Ready to receive from animals...");
  Serial.println("");
}

void loop() {
  // Handle incoming HTTP requests
  server.handleClient();

  // Keep WiFi connection alive
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected, reconnecting...");
    WiFi.reconnect();
  }

  delay(10);
}