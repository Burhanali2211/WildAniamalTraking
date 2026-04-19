// Configuration file for Animal Belt Tracking System
// Update these values according to your setup

export const SYSTEM_CONFIG = {
  // ==================== ARDUINO CONFIGURATION ====================
  DEVICE_NAME: "Billi_Meow",  // Change to your animal name
  DEVICE_ROLE: "CONTROLLER",  // ESP-NOW role
  ESP_SLEEP_TIME: 5e6,        // 5 seconds in microseconds
  BROADCAST_ADDRESS: "FF:FF:FF:FF:FF:FF",  // ESP-NOW broadcast

  // ==================== WiFi CONFIGURATION ====================
  WIFI_SSID: "Faheem",
  WIFI_PASSWORD: "12345678",
  
  // ==================== API CONFIGURATION ====================
  API_BASE_URL: "https://wild-aniamal-traking.vercel.app",
  API_UPDATE_ENDPOINT: "/api/update",
  API_DATA_ENDPOINT: "/api/data",

  // ==================== RSSI TO ZONE MAPPING ====================
  // RSSI = Received Signal Strength Indicator
  // Higher values (closer to 0) = stronger signal (closer range)
  // Lower values (more negative) = weaker signal (farther range)
  RSSI_THRESHOLDS: {
    STRONG: -50,   // SKICC_MainHall (< 10m)
    MEDIUM_HIGH: -60,  // SKICC_NorthLawn (10-20m)
    MEDIUM: -70,   // SKICC_SouthLawn (20-30m)
    WEAK: -80,     // SKICC_Parking (30-40m)
    VERY_WEAK: -100 // SKICC_Lakeside (40m+)
  },

  // ==================== ZONE COORDINATES ====================
  // Latitude, Longitude format
  ZONES: {
    "SKICC_MainHall": {
      coords: [34.0837, 74.8605],
      color: "#667eea"
    },
    "SKICC_NorthLawn": {
      coords: [34.0845, 74.8604],
      color: "#764ba2"
    },
    "SKICC_SouthLawn": {
      coords: [34.0829, 74.8606],
      color: "#f59e0b"
    },
    "SKICC_Parking": {
      coords: [34.0833, 74.8596],
      color: "#ef4444"
    },
    "SKICC_Lakeside": {
      coords: [34.0841, 74.8615],
      color: "#10b981"
    }
  },

  // ==================== DASHBOARD CONFIGURATION ====================
  FETCH_INTERVAL_MS: 3000,     // Update dashboard every 3 seconds
  REQUEST_TIMEOUT_MS: 5000,    // Wait max 5 seconds per request
  MAX_RETRIES: 3,              // Retry failed requests 3 times
  RETRY_DELAY_MS: 1000,        // Wait 1 second between retries
  LOG_MAX_ENTRIES: 20,         // Keep last 20 log entries

  // ==================== HARDWARE PIN CONFIGURATION ====================
  // For serverofanimal.ino ESP8266
  PINS: {
    BUZZER: 5,      // GPIO5 (D1)
    SWITCH: 0,      // GPIO0 (D3) - Pull-up mode
    TX: 1,          // Serial TX
    RX: 3           // Serial RX
  },

  // ==================== DEBUGGING ====================
  DEBUG_MODE: true,
  VERBOSE_LOGGING: true,
  SHOW_COORDINATES: true
};

// ==================== DERIVED CONFIGURATIONS ====================
export const DERIVED_CONFIG = {
  FULL_UPDATE_URL: SYSTEM_CONFIG.API_BASE_URL + SYSTEM_CONFIG.API_UPDATE_ENDPOINT,
  FULL_DATA_URL: SYSTEM_CONFIG.API_BASE_URL + SYSTEM_CONFIG.API_DATA_ENDPOINT,
  ZONE_NAMES: Object.keys(SYSTEM_CONFIG.ZONES),
  TOTAL_ZONES: Object.keys(SYSTEM_CONFIG.ZONES).length
};

// ==================== ZONE MAPPING FUNCTION ====================
export function mapRSSIToZone(rssi) {
  const { RSSI_THRESHOLDS } = SYSTEM_CONFIG;
  
  if (rssi > RSSI_THRESHOLDS.STRONG) return "SKICC_MainHall";
  if (rssi > RSSI_THRESHOLDS.MEDIUM_HIGH) return "SKICC_NorthLawn";
  if (rssi > RSSI_THRESHOLDS.MEDIUM) return "SKICC_SouthLawn";
  if (rssi > RSSI_THRESHOLDS.WEAK) return "SKICC_Parking";
  return "SKICC_Lakeside";
}

// ==================== VALIDATION ====================
export function validateConfig() {
  const errors = [];

  if (!SYSTEM_CONFIG.DEVICE_NAME) errors.push("DEVICE_NAME not set");
  if (!SYSTEM_CONFIG.WIFI_SSID) errors.push("WIFI_SSID not set");
  if (!SYSTEM_CONFIG.WIFI_PASSWORD) errors.push("WIFI_PASSWORD not set");
  if (!SYSTEM_CONFIG.API_BASE_URL) errors.push("API_BASE_URL not set");

  if (SYSTEM_CONFIG.FETCH_INTERVAL_MS < 1000) {
    errors.push("FETCH_INTERVAL_MS too low (minimum 1000ms)");
  }

  if (SYSTEM_CONFIG.REQUEST_TIMEOUT_MS < 1000) {
    errors.push("REQUEST_TIMEOUT_MS too low (minimum 1000ms)");
  }

  return {
    valid: errors.length === 0,
    errors
  };
}

export default SYSTEM_CONFIG;
