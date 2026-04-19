#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>

const char* ssid = "Faheem";
const char* password = "12345678";
const char* serverUrl = "https://wild-aniamal-traking.vercel.app/api/update";

#define BUZZER 5
#define SWITCH 0
#define SEND_TIMEOUT 30000  // 30 seconds max for HTTP request

typedef struct {
  char name[32];
  unsigned long timestamp;
} Message;

unsigned long lastServerUpdate = 0;
const unsigned long MIN_UPDATE_INTERVAL = 1000; // Minimum 1 second between updates

// ================= RSSI TO ZONE MAPPING =================
String getZoneFromRSSI(int rssi) {
  if (rssi > -50) return "SKICC_MainHall";
  if (rssi > -60) return "SKICC_NorthLawn";
  if (rssi > -70) return "SKICC_SouthLawn";
  if (rssi > -80) return "SKICC_Parking";
  return "SKICC_Lakeside";
}

// ================= SEND TO SERVER WITH RETRY =================
bool sendToServer(String name, int rssi) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected");
    return false;
  }

  // Rate limiting: don't send too frequently
  unsigned long now = millis();
  if (now - lastServerUpdate < MIN_UPDATE_INTERVAL) {
    return false;
  }

  HTTPClient http;
  http.setConnectTimeout(SEND_TIMEOUT);
  
  if (!http.begin(serverUrl)) {
    Serial.println("❌ HTTP begin failed");
    return false;
  }

  http.addHeader("Content-Type", "application/json");

  String zone = getZoneFromRSSI(rssi);
  String json = "{\"animal\":\"" + name + "\",\"zone\":\"" + zone + "\"}";

  Serial.print("📤 Sending: ");
  Serial.println(json);

  int httpResponseCode = http.POST(json);
  bool success = (httpResponseCode == 200);

  Serial.print("📨 Response: ");
  Serial.print(httpResponseCode);
  Serial.print(" | Animal: ");
  Serial.print(name);
  Serial.print(" | Zone: ");
  Serial.print(zone);
  Serial.print(" | RSSI: ");
  Serial.print(rssi);
  Serial.println(success ? " ✅" : " ❌");

  http.end();
  
  if (success) {
    lastServerUpdate = now;
  }
  
  return success;
}

// ================= ESP-NOW CALLBACK =================
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  Message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  String name = String(msg.name);
  int rssi = info->rx_ctrl->rssi;

  Serial.println("");
  Serial.print("🐾 Received: ");
  Serial.print(name);
  Serial.print(" | RSSI: ");
  Serial.println(rssi);

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

  // 🌐 Send to server
  sendToServer(name, rssi);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("");
  Serial.println("=== SKICC Wildlife Server ===");
  Serial.println("Initializing...");

  pinMode(BUZZER, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
  digitalWrite(BUZZER, LOW);

  // 🔴 CRITICAL: Set WiFi mode FIRST, before any ESP-NOW operations
  WiFi.mode(WIFI_STA);
  delay(100);

  // Initialize ESP-NOW BEFORE WiFi connection
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW Init Failed");
    return;
  }
  
  Serial.println("✅ ESP-NOW Initialized");
  esp_now_register_recv_cb(OnDataRecv);
  delay(100);
  
  // Now connect to WiFi for API communication
  Serial.print("📡 Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n⚠️ WiFi Connection Failed - ESP-NOW Still Active");
  }

  Serial.println("📍 Waiting for animals...");
}

void loop() {
  // Keep WiFi connection alive (but don't interfere with ESP-NOW)
  if (WiFi.status() != WL_CONNECTED) {
    delay(100);  // Small delay before attempting reconnect
    WiFi.reconnect();
  }
  
  delay(50);  // Reduced delay to ensure ESP-NOW responsiveness
}