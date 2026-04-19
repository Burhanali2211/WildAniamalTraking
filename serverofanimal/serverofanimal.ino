#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>

const char* ssid = "Faheem";
const char* password = "12345678";

// 🔗 Your Vercel API
const char* serverUrl = "https://wild-aniamal-traking.vercel.app/api/update";

#define BUZZER 5
#define SWITCH 0

typedef struct {
  char name[32];
} Message;

// ================= RSSI TO ZONE MAPPING =================
String getZoneFromRSSI(int rssi) {
  if (rssi > -50) return "SKICC_MainHall";
  if (rssi > -60) return "SKICC_NorthLawn";
  if (rssi > -70) return "SKICC_SouthLawn";
  if (rssi > -80) return "SKICC_Parking";
  return "SKICC_Lakeside";
}

// ================= SEND TO SERVER =================
void sendToServer(String name, int rssi) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String zone = getZoneFromRSSI(rssi);
    String json = "{\"animal\":\"" + name + "\",\"zone\":\"" + zone + "\"}";

    int httpResponseCode = http.POST(json);

    Serial.print("📨 Sent to server - Animal: ");
    Serial.print(name);
    Serial.print(" | Zone: ");
    Serial.print(zone);
    Serial.print(" | RSSI: ");
    Serial.print(rssi);
    Serial.print(" | Response: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("❌ WiFi not connected");
  }
}

// ================= ESP-NOW CALLBACK =================
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {

  Message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  String name = String(msg.name);
  int rssi = info->rx_ctrl->rssi;

  Serial.println("🐾 " + name + " | RSSI: " + String(rssi));

  // 🔊 buzzer
  if (digitalRead(SWITCH) == HIGH) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
  }

  // 🌐 SEND TO VERCEL
  sendToServer(name, rssi);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("📡 Ready...");
}

void loop() {}