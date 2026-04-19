#include <WiFi.h>
#include <esp_now.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ==================== CONFIGURATION ====================
const char* ssid = "Faheem";
const char* password = "12345678";

// CHANGE THIS FOR EACH OF YOUR 5 SERVERS:
// Options: "SKICC_MainHall", "SKICC_NorthLawn", "SKICC_SouthLawn", "SKICC_Parking", "SKICC_Lakeside"
const char* GATEWAY_ZONE = "SKICC_MainHall"; 

const char* vercelAPI = "https://wild-aniamal-traking.vercel.app/api/update";
// ========================================================

typedef struct struct_message {
    char animal[32];
} struct_message;

struct_message incomingData;
unsigned long lastVercelUpdate = 0;
const int MIN_INTERVAL = 2000; // 2 sec minimum between Vercel hits

void sendToVercel(String animal) {
    if (WiFi.status() != WL_CONNECTED) return;
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    Serial.println("\n🌐 Forwarding to Cloud: " + animal + " @ " + String(GATEWAY_ZONE));
    
    if (http.begin(client, vercelAPI)) {
        http.addHeader("Content-Type", "application/json");
        String json = "{\"animal\":\"" + animal + "\",\"zone\":\"" + String(GATEWAY_ZONE) + "\"}";
        
        int code = http.POST(json);
        if (code == 200) Serial.println("✅ Vercel Updated Successfully");
        else Serial.println("❌ Vercel Error: " + String(code));
        
        http.end();
    }
}

// Callback when data is received from animal
void OnDataRecv(const uint8_t * mac, const uint8_t *incoming, int len) {
    memcpy(&incomingData, incoming, sizeof(incomingData));
    String animalName = String(incomingData.animal);
    
    Serial.println("🐾 DETECTED: " + animalName + " nearby!");
    
    if (millis() - lastVercelUpdate > MIN_INTERVAL) {
        sendToVercel(animalName);
        lastVercelUpdate = millis();
    }
}

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\n✅ Gateway WiFi Connected. IP: " + WiFi.localIP().toString());

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("🏛️ Gateway Node Active: " + String(GATEWAY_ZONE));
}

void loop() {
    // Keep WiFi alive
    if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
    delay(100);
}