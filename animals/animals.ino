#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEVICE_NAME "Billi_Meow"
#define BROADCAST_INTERVAL 3000  // Send every 3 seconds (matches dashboard fetch)

typedef struct {
  char name[32];
  unsigned long timestamp;
} Message;

Message msg;
unsigned long lastSend = 0;

uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void OnDataSent(uint8_t *mac_addr, uint8_t status) {
  if (status == 0) {
    Serial.println("✅ ESP-NOW Send Success");
  } else {
    Serial.println("❌ ESP-NOW Send Failed");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, true); // Turn off WiFi to save power
  delay(200);

  if (esp_now_init() != 0) {
    Serial.println("❌ ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  strcpy(msg.name, DEVICE_NAME);
  Serial.println("=== SKICC Wildlife Tracker ===");
  Serial.println("📡 Broadcasting: " + String(DEVICE_NAME));
  Serial.println("Interval: " + String(BROADCAST_INTERVAL) + "ms");
}

void loop() {
  unsigned long now = millis();
  
  if (now - lastSend >= BROADCAST_INTERVAL) {
    msg.timestamp = now;
    
    int result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
    
    Serial.print("[");
    Serial.print(now / 1000);
    Serial.print("s] 📡 Broadcasting: ");
    Serial.print(msg.name);
    Serial.print(" | Result: ");
    Serial.println(result == 0 ? "✅" : "❌");
    
    lastSend = now;
  }
}