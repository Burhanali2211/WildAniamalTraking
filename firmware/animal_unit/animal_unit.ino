#include <espnow.h>
#include <ESP8266WiFi.h>

#define DEVICE_NAME "Billi_Meow"
#define BROADCAST_INTERVAL 3000  // Send every 3 seconds

// Structure to send data
typedef struct struct_message {
    char animal[32];
} struct_message;

struct_message myData;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("❌ Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  
  // Register broadcast peer
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  strcpy(myData.animal, DEVICE_NAME);
  Serial.println("✅ Animal Unit Ready (Broadcast Mode)");
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= BROADCAST_INTERVAL) {
    esp_now_send(NULL, (uint8_t *) &myData, sizeof(myData));
    Serial.println("📡 Broadcast Sent: " + String(DEVICE_NAME));
    lastSend = now;
  }
}