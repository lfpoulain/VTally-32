/*
 * VTally-32 - Version 2.0
 * 
 * Système de tally light professionnel pour VMix
 * - Gestion WiFi native ESP32 avec retry automatique
 * - Protocole TCP VMix officiel avec reconnexion
 * - Interface web moderne avec onglets et scan WiFi
 * - Configuration persistante et validation
 * - Support NeoPixel RGB avec personnalisation
 * 
 * Auteur: LFPoulain
 * License: MIT
 * Version: 2.0.0
 * GitHub: https://github.com/lfpoulain/VTally-32
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <lwip/sockets.h>

// ========================================
// Configuration et constantes
// ========================================
#define WIFI_RETRY_COUNT 3
#define WIFI_RETRY_TIMEOUT 5
#define WIFI_CHECK_INTERVAL 5000
#define VMIX_CHECK_INTERVAL 1000
#define VMIX_MAX_RETRY_INTERVAL 5000
#define VMIX_RESPONSE_TIMEOUT 1200
#define VMIX_TCP_TIMEOUT 5000
#define STATUS_UPDATE_INTERVAL 1000
#define WIFI_STATUS_CACHE_MS 200

// Niveaux de log
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_PRODUCTION 2
#define LOG_LEVEL_DEBUG 3
#define DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG

// Constantes WiFi AP
#define AP_PASSWORD "vtally32"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 4

// Constantes par défaut
#define DEFAULT_TALLY_NAME "VTally-32"
#define DEFAULT_VMIX_HOST "192.168.1.100"
#define DEFAULT_VMIX_INPUT "1"
#define DEFAULT_LIVE_COLOR 0xFF0000
#define DEFAULT_PREVIEW_COLOR 0x00FF00
#define DEFAULT_OFF_COLOR 0x000000
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_LED_PIN 14
#define DEFAULT_LED_COUNT 1
#define DISPLAY_MODE_SINGLE 0
#define DISPLAY_MODE_MATRIX_8X8 1

const char* FIRMWARE_VERSION = "2.1.0";

// ========================================
// Macros de logging conditionnelles
// ========================================
#define LOG_ERROR(msg, ...) do { if (config.log_level >= LOG_LEVEL_ERROR) Serial.printf("[ERROR] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_WARN(msg, ...) do { if (config.log_level >= LOG_LEVEL_ERROR) Serial.printf("[WARN] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_INFO(msg, ...) do { if (config.log_level >= LOG_LEVEL_PRODUCTION) Serial.printf("[INFO] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_NETWORK(msg, ...) do { if (config.log_level >= LOG_LEVEL_PRODUCTION) Serial.printf("[NET] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_VMIX(msg, ...) do { if (config.log_level >= LOG_LEVEL_PRODUCTION) Serial.printf("[VMIX] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_WEB(msg, ...) do { if (config.log_level >= LOG_LEVEL_PRODUCTION) Serial.printf("[WEB] " msg "\n", ##__VA_ARGS__); } while(0)
#define LOG_DEBUG(msg, ...) do { if (config.log_level >= LOG_LEVEL_DEBUG) Serial.printf("[DEBUG] " msg "\n", ##__VA_ARGS__); } while(0)

// ========================================
// Variables globales
// ========================================
WebServer server(80);
Adafruit_NeoPixel *strip = nullptr;
Preferences preferences;

struct Config {
  // Config Générale
  char tally_name[33] = DEFAULT_TALLY_NAME;

  // Config WiFi
  char wifi_ssid[33] = "";
  char wifi_password[64] = "";
  
  // Config VMix
  char vmix_host[40] = DEFAULT_VMIX_HOST;
  char vmix_input[10] = DEFAULT_VMIX_INPUT;

  // Config Couleurs
  uint32_t live_color = DEFAULT_LIVE_COLOR;
  uint32_t preview_color = DEFAULT_PREVIEW_COLOR;
  uint32_t off_color = DEFAULT_OFF_COLOR;
  int brightness = DEFAULT_BRIGHTNESS;

  // Config Matérielle (Nécessite un redémarrage)
  int led_pin = DEFAULT_LED_PIN;
  int led_count = DEFAULT_LED_COUNT;
  uint8_t display_mode = DISPLAY_MODE_SINGLE;
  bool live_debug = false;

  // Config Performance
  uint8_t log_level = DEFAULT_LOG_LEVEL;
} config;

WiFiClient vmixClient;
unsigned long lastVMixCheck = 0;
unsigned long vmixCheckInterval = VMIX_CHECK_INTERVAL;
unsigned long lastWiFiCheck = 0;
unsigned long wifiConnectAttemptStart = 0;
bool wifiConnectInProgress = false;
bool vmixConnected = false;
bool apActive = false;
bool isLive = false;
bool isPreview = false;
String lastState = "OFF";
int lastBrightness = -1;
bool otaUploadStarted = false;
bool otaUploadSuccess = false;
String otaLastError = "";
char vmixLineBuffer[260];
int vmixLineBufferLen = 0;
uint8_t debugStageCode = 0;
String debugStageLabel = "BOOT";

// Variables pour le redémarrage propre
bool pendingReboot = false;
unsigned long rebootTime = 0;

// Cache WiFi.status() pour éviter appels répétés au driver
wl_status_t cachedWiFiStatus = WL_DISCONNECTED;
unsigned long lastWiFiStatusCheck = 0;

wl_status_t getWiFiStatusCached() {
  unsigned long now = millis();
  if (now - lastWiFiStatusCheck >= WIFI_STATUS_CACHE_MS) {
    cachedWiFiStatus = WiFi.status();
    lastWiFiStatusCheck = now;
  }
  return cachedWiFiStatus;
}

// ========================================
// Setup & Loop
// ========================================

void setup() {
  Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
  unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart) < 2000) {
    delay(10);
  }
#endif
  delay(2000);
  
  LOG_INFO("\n=============================");
  LOG_INFO("  VTally-32 v%s", FIRMWARE_VERSION);
  LOG_INFO("=============================");
  
  // Configuration
  loadConfig();
  
  // LED (initialisation après chargement config)
  strip = new Adafruit_NeoPixel(config.led_count, config.led_pin, NEO_RGB + NEO_KHZ800);
  strip->begin();
  strip->show();
  applyTallyState();
  
  // WiFi
  setupWiFi();
  esp_wifi_set_ps(WIFI_PS_NONE);

  // VMix
  if (getWiFiStatusCached() == WL_CONNECTED) {
    connectVMix();
  }
  
  // Serveur web
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/wifi", handleWiFi);
  server.on("/scan", handleScan);
  server.on("/status", handleStatus);
  server.on("/diagnostics", handleDiagnostics);
  server.on("/reboot", HTTP_POST, handleReboot);
  server.on("/vmix/reconnect", HTTP_POST, handleVMixReconnect);
  server.on("/update", HTTP_POST, handleOTAUpdate, handleOTAUpload);
  server.on("/favicon.ico", []() {
    server.send(204);
  });
  
  server.onNotFound([]() {
    LOG_WARN("Page non trouvée (404) : %s", server.uri().c_str());
    server.send(404, "text/plain", "404: Not Found");
  });

  server.begin();
  
  LOG_INFO("\nSystème prêt!");
  LOG_INFO("=============================");
}

void loop() {
  // Gestion du redémarrage asynchrone propre
  if (pendingReboot) {
    if (millis() - rebootTime > 1000) {
      LOG_INFO("Redémarrage matériel maintenant...");
      ESP.restart();
    }
    server.handleClient(); // Laisser le serveur répondre pendant l'attente
    return;
  }

  // Priorité 1 : Traiter les données tally avec latence minimale
  if (vmixConnected && vmixClient.available()) {
    handleVMix();
  }

  server.handleClient();

  // Surveillance WiFi et réactivation AP si nécessaire
  checkWiFi();

  if (getWiFiStatusCached() == WL_CONNECTED) {
    checkVMix();
  }

  delay(1);
}
