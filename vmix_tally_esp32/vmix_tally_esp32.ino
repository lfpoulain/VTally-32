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
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>

// ========================================
// Configuration et constantes
// ========================================
#define WIFI_RETRY_COUNT 3
#define WIFI_RETRY_TIMEOUT 5
#define WIFI_CHECK_INTERVAL 30000
#define VMIX_CHECK_INTERVAL 5000
#define VMIX_RESPONSE_TIMEOUT 3000
#define VMIX_TCP_TIMEOUT 5000
#define STATUS_UPDATE_INTERVAL 1000

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

// ========================================
// Macros de logging optimisées
// ========================================
#define LOG_INFO(msg, ...) Serial.printf("[INFO] " msg "\n", ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) Serial.printf("[ERROR] " msg "\n", ##__VA_ARGS__)
#define LOG_WARN(msg, ...) Serial.printf("[WARN] " msg "\n", ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) Serial.printf("[DEBUG] " msg "\n", ##__VA_ARGS__)
#define LOG_NETWORK(msg, ...) Serial.printf("[NET] " msg "\n", ##__VA_ARGS__)
#define LOG_VMIX(msg, ...) Serial.printf("[VMIX] " msg "\n", ##__VA_ARGS__)
#define LOG_WEB(msg, ...) Serial.printf("[WEB] " msg "\n", ##__VA_ARGS__)

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
} config;

WiFiClient vmixClient;
unsigned long lastVMixCheck = 0;
unsigned long vmixCheckInterval = VMIX_CHECK_INTERVAL;
unsigned long lastWiFiCheck = 0;
bool vmixConnected = false;
bool apActive = false;
bool isLive = false;
bool isPreview = false;
String lastState = "OFF";
int lastBrightness = -1;

// Variables pour le redémarrage propre
bool pendingReboot = false;
unsigned long rebootTime = 0;

// ========================================
// Setup & Loop
// ========================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  LOG_INFO("\n=============================");
  LOG_INFO("  VTally-32 v2.0.0");
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
  
  // VMix
  if (WiFi.status() == WL_CONNECTED) {
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

  server.handleClient();
  
  // Surveillance WiFi et réactivation AP si nécessaire
  checkWiFi();
  
  if (WiFi.status() == WL_CONNECTED) {
    checkVMix();
    
    // Traiter les données VMix si connecté
    if (vmixClient.connected()) {
      handleVMix();
    }
  }
  
  delay(10);
}
