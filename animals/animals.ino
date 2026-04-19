#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEVICE_NAME "Billi_Meow"
#define SLEEP_TIME 5e6  // 5 seconds

typedef struct {
  char name[32];
} Message;

Message msg;

uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void OnDataSent(uint8_t *mac_addr, uint8_t status) {
  if (status == 0) {
    Serial.println("✅ Send Success");
  } else {
    Serial.println("❌ Send Failed");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  delay(200);   // 🔥 REQUIRED

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  strcpy(msg.name, DEVICE_NAME);
  Serial.println("📡 Ready to broadcast...");
  Serial.println(msg.name);
}

void loop() {
  esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
  Serial.println("📡 Broadcasting: " + String(msg.name));
  delay(5000);  // Send every 5 seconds
}