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
#define AP_SSID "VTally-32"
#define AP_PASSWORD "vtally32"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 4

// Constantes par défaut
#define DEFAULT_VMIX_HOST "192.168.1.100"
#define DEFAULT_VMIX_PORT 8088
#define VMIX_TCP_PORT 8099
#define DEFAULT_VMIX_INPUT "1"
#define DEFAULT_LIVE_COLOR 0xFF0000
#define DEFAULT_PREVIEW_COLOR 0x00FF00
#define DEFAULT_OFF_COLOR 0x000000
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_LED_PIN 14
#define DEFAULT_LED_COUNT 1
#define DEFAULT_VMIX_TRACK_BY_KEY false
#define DEFAULT_VMIX_KEY_REFRESH_SECONDS 10

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
  // Config WiFi
  char wifi_ssid[33] = "";
  char wifi_password[64] = "";
  
  // Config VMix
  char vmix_host[40] = DEFAULT_VMIX_HOST;
  int vmix_port = DEFAULT_VMIX_PORT;
  char vmix_input[10] = DEFAULT_VMIX_INPUT;
  bool vmix_track_by_key = DEFAULT_VMIX_TRACK_BY_KEY;
  char vmix_input_key[48] = "";
  char vmix_input_title[96] = "";
  int vmix_key_refresh_seconds = DEFAULT_VMIX_KEY_REFRESH_SECONDS;

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
unsigned long lastWiFiCheck = 0;
bool vmixConnected = false;
bool apActive = false;
bool isLive = false;
bool isPreview = false;
String lastState = "OFF";
int lastBrightness = -1;
unsigned long lastVMixInputRefresh = 0;
String lastVMixTallyData = "";
int resolvedVMixInputNumber = 0;
String resolvedVMixInputKey = "";
String resolvedVMixInputTitle = "";

#if 0
// ========================================
// Gestion de la configuration
// ========================================

void saveConfig() {
  LOG_INFO("Sauvegarde configuration...");
  preferences.begin("vtally32", false);  // false = lecture/écriture
  
  // Sauvegarder chaque paramètre individuellement pour cohérence avec loadConfig
  preferences.putString("wifi_ssid", config.wifi_ssid);
  preferences.putString("wifi_pwd", config.wifi_password);
  preferences.putString("host", config.vmix_host);
  preferences.putInt("port", config.vmix_port);
  preferences.putString("input", config.vmix_input);
  preferences.putUInt("live", config.live_color);
  preferences.putUInt("preview", config.preview_color);
  preferences.putUInt("off", config.off_color);
  preferences.putInt("brightness", config.brightness);
  preferences.putInt("led_pin", config.led_pin);
  preferences.putInt("led_count", config.led_count);
  
  preferences.end();
  
  LOG_DEBUG("Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X", 
            config.live_color, config.preview_color, config.off_color);
  LOG_DEBUG("VMix: %s:%d Input:%s", 
            config.vmix_host, config.vmix_port, config.vmix_input);
  LOG_DEBUG("LED: Pin=%d Count=%d Brightness=%d", 
            config.led_pin, config.led_count, config.brightness);
  LOG_DEBUG("WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Non configuré");
  LOG_INFO("Configuration sauvegardée");
}

void loadConfig() {
  preferences.begin("vtally32", true);
  
  strlcpy(config.wifi_ssid, preferences.getString("wifi_ssid", "").c_str(), sizeof(config.wifi_ssid));
  strlcpy(config.wifi_password, preferences.getString("wifi_pwd", "").c_str(), sizeof(config.wifi_password));
  strlcpy(config.vmix_host, preferences.getString("host", DEFAULT_VMIX_HOST).c_str(), sizeof(config.vmix_host));
  config.vmix_port = preferences.getInt("port", DEFAULT_VMIX_PORT);
  strlcpy(config.vmix_input, preferences.getString("input", DEFAULT_VMIX_INPUT).c_str(), sizeof(config.vmix_input));
  config.live_color = preferences.getUInt("live", DEFAULT_LIVE_COLOR);
  config.preview_color = preferences.getUInt("preview", DEFAULT_PREVIEW_COLOR);
  config.off_color = preferences.getUInt("off", DEFAULT_OFF_COLOR);
  config.brightness = preferences.getInt("brightness", DEFAULT_BRIGHTNESS);
  config.led_pin = preferences.getInt("led_pin", DEFAULT_LED_PIN);
  config.led_count = preferences.getInt("led_count", DEFAULT_LED_COUNT);

  if (strlen(config.vmix_host) == 0) {
    strlcpy(config.vmix_host, DEFAULT_VMIX_HOST, sizeof(config.vmix_host));
  }
  if (!isValidPort(config.vmix_port)) {
    config.vmix_port = DEFAULT_VMIX_PORT;
  }
  if (!isValidVMixInput(config.vmix_input)) {
    strlcpy(config.vmix_input, DEFAULT_VMIX_INPUT, sizeof(config.vmix_input));
  }
  if (!isValidBrightness(config.brightness)) {
    config.brightness = DEFAULT_BRIGHTNESS;
  }
  if (!isValidLedPin(config.led_pin)) {
    config.led_pin = DEFAULT_LED_PIN;
  }
  if (!isValidLedCount(config.led_count)) {
    config.led_count = DEFAULT_LED_COUNT;
  }
  
  preferences.end();
  
  LOG_INFO("Configuration chargée:");
  LOG_DEBUG("  WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Non configuré");
  LOG_DEBUG("  VMix: %s:%d Input:%s", config.vmix_host, config.vmix_port, config.vmix_input);
  LOG_DEBUG("  Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X", 
            config.live_color, config.preview_color, config.off_color);
}

// ========================================
// Gestion WiFi
// ========================================

void startAP() {
  WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
  apActive = true;
  LOG_NETWORK("AP démarré: http://%s", WiFi.softAPIP().toString().c_str());
}

void setupWiFi() {
  LOG_NETWORK("Configuration WiFi en cours...");
  WiFi.mode(WIFI_AP_STA);
  
  // Point d'accès temporaire pour configuration
  startAP();
  
  // Connexion au réseau configuré si credentials disponibles
  if (strlen(config.wifi_ssid) > 0) {
    bool connected = false;
    
    for (int attempt = 1; attempt <= WIFI_RETRY_COUNT && !connected; attempt++) {
      LOG_NETWORK("Tentative %d/%d - Connexion à: %s", attempt, WIFI_RETRY_COUNT, config.wifi_ssid);
      WiFi.begin(config.wifi_ssid, config.wifi_password);
      
      unsigned long start = millis();
      while (WiFi.status() != WL_CONNECTED && (millis() - start) < (WIFI_RETRY_TIMEOUT * 1000)) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
      
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        LOG_NETWORK("WiFi connecté: %s (%s)", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        
        // WiFi connecté : désactiver l'AP
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        apActive = false;
        LOG_NETWORK("AP désactivé (WiFi connecté)");
      } else {
        LOG_WARN("Échec tentative %d/%d", attempt, WIFI_RETRY_COUNT);
        if (attempt < WIFI_RETRY_COUNT) {
          WiFi.disconnect();
          delay(1000);
        }
      }
    }
    
    if (!connected) {
      LOG_ERROR("Échec connexion WiFi après %d tentatives - Mode AP uniquement", WIFI_RETRY_COUNT);
    }
  } else {
    LOG_INFO("Pas de WiFi configuré - Mode AP uniquement");
  }
}

void checkWiFi() {
  // Vérifier périodiquement l'état WiFi
  if (millis() - lastWiFiCheck < WIFI_CHECK_INTERVAL) return;
  lastWiFiCheck = millis();
  
  if (WiFi.status() == WL_CONNECTED) {
    // WiFi connecté : s'assurer que l'AP est désactivé
    if (apActive) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      apActive = false;
      LOG_NETWORK("AP désactivé (WiFi reconnecté)");
    }
  } else {
    // WiFi déconnecté : tenter reconnexion
    if (strlen(config.wifi_ssid) > 0) {
      LOG_NETWORK("Tentative reconnexion WiFi...");
      WiFi.begin(config.wifi_ssid, config.wifi_password);
    }

    if (vmixClient.connected() || vmixConnected) {
      vmixClient.stop();
      vmixConnected = false;
      setTally(false, false);
    }
    
    // Réactiver l'AP pour reconfiguration si pas déjà actif
    if (!apActive) {
      WiFi.mode(WIFI_AP_STA);
      startAP();
      LOG_NETWORK("AP réactivé (WiFi perdu)");
    }
  }
}

// ========================================
// Gestion VMix
// ========================================

bool connectVMix() {
  if (vmixClient.connected()) {
    LOG_VMIX("Déjà connecté à VMix");
    return true;
  }
  
  LOG_VMIX("Connexion à VMix %s:%d...", config.vmix_host, config.vmix_port);
  
  vmixClient.setNoDelay(true);
  vmixClient.setTimeout(VMIX_TCP_TIMEOUT);
  
  if (vmixClient.connect(config.vmix_host, config.vmix_port)) {
    delay(100);
    
    if (!vmixClient.connected()) {
      LOG_ERROR("Connexion fermée par VMix");
      return false;
    }
    
    // Abonnement aux événements TALLY
    vmixClient.print("SUBSCRIBE TALLY\r\n");
    vmixClient.flush();
    
    // Attente de la réponse (VMix peut envoyer TALLY avant SUBSCRIBE OK)
    unsigned long start = millis();
    bool success = false;
    
    while (millis() - start < VMIX_RESPONSE_TIMEOUT && !success) {
      if (vmixClient.available()) {
        String response = vmixClient.readStringUntil('\n');
        response.trim();
        
        if (response.startsWith("SUBSCRIBE OK") || response.startsWith("TALLY OK")) {
          success = true;
          LOG_VMIX("Réponse reçue: %s", response.c_str());
          if (response.startsWith("TALLY OK")) {
            parseVMix(response);
          }
        }
      }
      
      if (!vmixClient.connected()) {
        LOG_VMIX("VMix a fermé la connexion");
        return false;
      }
      
      delay(10);
    }
    
    if (success) {
      vmixConnected = true;
      LOG_VMIX("VMix connecté et abonné avec succès");
      return true;
    } else {
      LOG_ERROR("VMix n'a pas répondu - déconnexion");
      vmixClient.stop();
      return false;
    }
  } else {
    LOG_ERROR("Échec connexion VMix");
    return false;
  }
}

void checkVMix() {
  if (millis() - lastVMixCheck < VMIX_CHECK_INTERVAL) return;
  lastVMixCheck = millis();
  
  if (!vmixClient.connected()) {
    if (vmixConnected) {
      LOG_VMIX("Connexion VMix perdue");
      vmixConnected = false;
      setTally(false, false);
    }
    connectVMix();
  }
}

void handleVMix() {
  while (vmixClient.available()) {
    String line = vmixClient.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      LOG_DEBUG("VMix reçu: %s", line.c_str());
      parseVMix(line);
    }
  }
}

// ========================================
// Gestion LED
// ========================================

void setTally(bool live, bool preview) {
  if (live == isLive && preview == isPreview) return;
  
  LOG_DEBUG("Tally: %s → Live:%s Preview:%s", 
    (isLive ? "LIVE" : (isPreview ? "PREVIEW" : "OFF")),
    (live ? "OUI" : "NON"),
    (preview ? "OUI" : "NON"));
  
  isLive = live;
  isPreview = preview;
  
  String state = "OFF";
  
  if (live && preview) {
    state = "LIVE+PREVIEW";
  } else if (live) {
    state = "LIVE";
  } else if (preview) {
    state = "PREVIEW";
  }
  
  if (state != lastState) {
    LOG_INFO("TALLY: %s → %s [Input %s]", lastState.c_str(), state.c_str(), config.vmix_input);
    lastState = state;
  }

  applyTallyState();
}

// ========================================
// Parsing VMix
// ========================================

void parseVMix(const String& cmd) {
  if (!cmd.startsWith("TALLY OK")) return;
  
  // Format: TALLY OK 0121... (chaque caractère = état d'un input)
  // 0 = off, 1 = program (live), 2 = preview
  
  // "TALLY OK " = 9 caractères
  if (cmd.length() < 10) return;
  
  String tallyData = cmd.substring(9);
  tallyData.trim();
  
  // Validation de l'input VMix configuré
  if (strlen(config.vmix_input) == 0) {
    LOG_WARN("Input VMix non configuré");
    return;
  }
  
  // Convertir l'input configuré en index (input 1 = index 0)
  int inputIndex = atoi(config.vmix_input) - 1;
  
  // Valider que l'index est dans la plage valide
  if (inputIndex < 0) {
    LOG_WARN("Input VMix invalide: %s", config.vmix_input);
    return;
  }
  
  if (inputIndex < tallyData.length()) {
    char state = tallyData.charAt(inputIndex);
    
    // 0 = off, 1 = program (live), 2 = preview
    bool live = (state == '1' || state == '3');
    bool preview = (state == '2' || state == '3');
    
    setTally(live, preview);
  } else {
    LOG_WARN("Input %s hors limites (max: %d)", config.vmix_input, tallyData.length());
  }
}

// ========================================
// Interface Web
// ========================================

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html.reserve(16384);
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  html += "<title>VTally-32</title>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box}";
  html += "body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#0a0e27;background:linear-gradient(135deg,#0a0e27 0%,#1a1535 50%,#2d1b4e 100%);color:#e0e7ff;min-height:100vh;padding:20px;position:relative;overflow-x:hidden}";
  html += "body::before{content:'';position:absolute;top:0;left:0;right:0;bottom:0;background:radial-gradient(circle at 20% 50%,rgba(99,102,241,0.1) 0%,transparent 50%),radial-gradient(circle at 80% 80%,rgba(168,85,247,0.1) 0%,transparent 50%);pointer-events:none}";
  html += ".container{max-width:800px;margin:0 auto;position:relative;z-index:1}";
  html += ".card{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3),inset 0 1px 0 rgba(255,255,255,0.05);transition:all 0.3s ease}";
  html += ".card:hover{border-color:rgba(99,102,241,0.4);box-shadow:0 12px 48px rgba(99,102,241,0.15),inset 0 1px 0 rgba(255,255,255,0.05)}";
  html += ".header{background:rgba(15,23,42,0.6);backdrop-filter:blur(20px);border-radius:20px;padding:32px 28px;margin-bottom:24px;border:1px solid rgba(99,102,241,0.2);box-shadow:0 8px 32px rgba(0,0,0,0.3);transition:border 0.3s ease;position:relative;overflow:hidden;--status-color:#6366f1}";
  html += ".header::before{content:'';position:absolute;top:0;left:0;right:0;height:4px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transition:all 0.15s ease-out}";
  html += ".header.status-live::before,.header.status-preview::before{background:var(--status-color);box-shadow:0 0 20px var(--status-color)}";
  html += "h1{text-align:center;margin-bottom:8px;font-size:2.2em;background:linear-gradient(135deg,#818cf8 0%,#c084fc 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;font-weight:700;letter-spacing:-0.5px;transition:filter 0.15s ease-out}";
  html += ".header.status-live h1,.header.status-preview h1{background:var(--status-color);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;filter:brightness(1.2)}";
  html += ".subtitle{text-align:center;color:#94a3b8;font-size:0.85em;font-weight:500;margin-bottom:8px}";
  html += ".status-badge{display:inline-block;padding:6px 16px;border-radius:20px;font-size:0.8em;font-weight:600;margin-top:12px;background:rgba(99,102,241,0.2);border:1px solid rgba(99,102,241,0.4);color:#c7d2fe;transition:all 0.15s ease-out}";
  html += ".header.status-live .status-badge,.header.status-preview .status-badge{background:color-mix(in srgb,var(--status-color) 20%,transparent);border-color:color-mix(in srgb,var(--status-color) 40%,transparent);color:var(--status-color);filter:brightness(1.3)}";
  html += ".form-group{margin-bottom:22px}";
  html += "label{display:block;margin-bottom:10px;font-weight:500;color:#cbd5e1;font-size:0.9em;letter-spacing:0.3px}";
  html += "input,select{width:100%;padding:14px 16px;border:1px solid rgba(99,102,241,0.3);border-radius:12px;background:rgba(15,23,42,0.8);color:#e0e7ff;font-size:15px;transition:all 0.3s;outline:none}";
  html += "input:focus,select:focus{border-color:rgba(99,102,241,0.6);background:rgba(15,23,42,0.95);box-shadow:0 0 0 3px rgba(99,102,241,0.1)}";
  html += "input::placeholder{color:#64748b}";
  html += "input[readonly]{background:rgba(15,23,42,0.5);color:#94a3b8;cursor:not-allowed}";
  html += ".color-input{height:56px;cursor:pointer;border-width:2px}";
  html += ".color-input:hover{border-color:rgba(168,85,247,0.5)}";
  html += ".btn{background:linear-gradient(135deg,#6366f1 0%,#8b5cf6 100%);color:#fff;padding:14px 28px;border:none;border-radius:12px;cursor:pointer;font-size:15px;font-weight:600;width:100%;margin-bottom:12px;transition:all 0.3s;box-shadow:0 4px 16px rgba(99,102,241,0.3);position:relative;overflow:hidden}";
  html += ".btn::before{content:'';position:absolute;top:50%;left:50%;width:0;height:0;border-radius:50%;background:rgba(255,255,255,0.2);transform:translate(-50%,-50%);transition:width 0.6s,height 0.6s}";
  html += ".btn:hover::before{width:300px;height:300px}";
  html += ".btn:hover{transform:translateY(-2px);box-shadow:0 6px 24px rgba(99,102,241,0.4)}";
  html += ".btn:active{transform:translateY(0)}";
  html += ".btn-danger{background:linear-gradient(135deg,#ef4444 0%,#dc2626 100%);box-shadow:0 4px 16px rgba(239,68,68,0.3)}";
  html += ".btn-danger:hover{box-shadow:0 6px 24px rgba(239,68,68,0.4)}";
  html += ".btn-warning{background:linear-gradient(135deg,#f59e0b 0%,#d97706 100%);box-shadow:0 4px 16px rgba(245,158,11,0.3)}";
  html += ".btn-warning:hover{box-shadow:0 6px 24px rgba(245,158,11,0.4)}";
  html += "h3{font-size:1.3em;margin:24px 0 20px;padding-bottom:12px;border-bottom:2px solid rgba(99,102,241,0.3);color:#c7d2fe;font-weight:600;letter-spacing:-0.3px}";
  html += ".tabs{display:flex;gap:12px;margin-bottom:24px;flex-wrap:wrap}";
  html += ".tab{background:rgba(15,23,42,0.5);padding:14px 28px;border-radius:12px;cursor:pointer;transition:all 0.3s;border:2px solid rgba(99,102,241,0.2);font-weight:500;color:#94a3b8;position:relative}";
  html += ".tab::after{content:'';position:absolute;bottom:0;left:50%;width:0;height:2px;background:linear-gradient(90deg,#6366f1,#8b5cf6);transform:translateX(-50%);transition:width 0.3s}";
  html += ".tab:hover{background:rgba(15,23,42,0.8);border-color:rgba(99,102,241,0.4);color:#c7d2fe}";
  html += ".tab.active{background:rgba(99,102,241,0.15);border-color:rgba(99,102,241,0.5);color:#e0e7ff;box-shadow:0 4px 16px rgba(99,102,241,0.2)}";
  html += ".tab.active::after{width:80%}";
  html += ".tab-content{display:none;animation:fadeIn 0.4s ease}";
  html += ".tab-content.active{display:block}";
  html += "@keyframes fadeIn{from{opacity:0;transform:translateY(10px)}to{opacity:1;transform:translateY(0)}}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='header' id='header'>";
  html += "<h1>VTally-32</h1>";
  html += "<div class='subtitle' id='info'>Professional VMix Tally System</div>";
  html += "<div style='text-align:center'><span class='status-badge' id='statusBadge'>Démarrage...</span></div>";
  html += "</div>";

  html += "<div class='tabs'>";
  html += "<div class='tab active' onclick='switchTab(0)'>VMix & Couleurs</div>";
  html += "<div class='tab' onclick='switchTab(1)'>Matériel</div>";
  html += "<div class='tab' onclick='switchTab(2)'>WiFi</div>";
  html += "</div>";

  html += "<div class='tab-content active' id='tab0'>";
  html += "<div class='card'>";
  html += "<h3>Configuration VMix</h3>";
  html += "<form id='vmixForm'>";
  html += "<div class='form-group'><label>IP VMix</label><input type='text' id='host' required pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'></div>";
  html += "<div class='form-group'><label>Port (8088 pour VMix 24+, 8099 pour anciennes versions)</label><input type='number' id='port' min='1' max='65535' required></div>";
  html += "<div class='form-group'><label>Input Number</label><input type='number' id='input' min='1' max='999' required></div>";
  
  html += "<h3>Couleurs</h3>";
  html += "<div class='form-group'><label>Live</label><input type='color' id='live' class='color-input'></div>";
  html += "<div class='form-group'><label>Preview</label><input type='color' id='preview' class='color-input'></div>";
  html += "<div class='form-group'><label>Off</label><input type='color' id='off' class='color-input'></div>";
  html += "<div class='form-group'><label>Luminosite: <span id='bval'>255</span></label><input type='range' id='brightness' min='0' max='255' value='255'></div>";
  
  html += "<button type='submit' class='btn'>Sauvegarder Configuration</button>";
  html += "</form></div></div>";

  html += "<div class='tab-content' id='tab1'>";
  html += "<div class='card'>";
  html += "<h3>Configuration Matérielle</h3>";
  html += "<p style='background:rgba(245,158,11,0.1);padding:16px 18px;border-radius:12px;margin-bottom:24px;border:1px solid rgba(245,158,11,0.3);color:#fcd34d;font-size:0.9em;line-height:1.6'><strong style='color:#fbbf24'>⚠️ Attention:</strong> Toute modification nécessite un redémarrage automatique de l'ESP32.</p>";
  html += "<form id='hardwareForm'>";
  html += "<div class='form-group'><label>Pin GPIO de la LED</label><input type='number' id='led_pin' min='0' max='48' required></div>";
  html += "<div class='form-group'><label>Nombre de LEDs</label><input type='number' id='led_count' min='1' max='255' required></div>";
  html += "<button type='submit' class='btn btn-warning'>Sauvegarder et Redémarrer</button>";
  html += "</form></div></div>";

  html += "<div class='tab-content' id='tab2'>";
  html += "<div class='card'>";
  html += "<h3>Configuration WiFi</h3>";
  html += "<div class='form-group'><label>Réseaux disponibles</label><select id='wifiList' onchange='selectNetwork()'><option value=''>Chargement...</option></select><button type='button' class='btn' onclick='scanWiFi()' style='margin-top:10px'>🔄 Scanner</button></div>";
  html += "<div class='form-group'><label>SSID (ou saisir manuellement)</label><input type='text' id='newssid' placeholder='Nom du réseau WiFi'></div>";
  html += "<div class='form-group'><label>Mot de passe</label><input type='password' id='pwd' placeholder='Mot de passe WiFi'></div>";
  html += "<button type='button' class='btn' onclick='saveWiFi()'>Configurer WiFi et Redémarrer</button>";
  html += "<button type='button' class='btn btn-danger' onclick='reboot()'>Redémarrer</button>";
  html += "</div></div></div>";

  html += "<script>";
  html += "let currentColors={live:'#ff0000',preview:'#00ff00',off:'#000000'};";

  html += "function toHex(c){let h=((c||0)>>>0).toString(16).padStart(6,'0');return '#'+h;}";

  html += "document.addEventListener('DOMContentLoaded',function(){";
  html += "fetch('/config').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('host').value=d.vmix_host;";
  html += "document.getElementById('port').value=d.vmix_port;";
  html += "document.getElementById('input').value=d.vmix_input;";
  html += "document.getElementById('brightness').value=d.brightness;";
  html += "document.getElementById('bval').textContent=d.brightness;";
  html += "document.getElementById('led_pin').value=d.led_pin;";
  html += "document.getElementById('led_count').value=d.led_count;";
  html += "currentColors.live=toHex(d.live_color);";
  html += "currentColors.preview=toHex(d.preview_color);";
  html += "currentColors.off=toHex(d.off_color);";
  html += "document.getElementById('live').value=currentColors.live;";
  html += "document.getElementById('preview').value=currentColors.preview;";
  html += "document.getElementById('off').value=currentColors.off;";
  html += "updateStatus();";
  html += "scanWiFi();";
  html += "});";

  html += "document.getElementById('live').addEventListener('input',e=>{currentColors.live=e.target.value;updateStatus();});";
  html += "document.getElementById('preview').addEventListener('input',e=>{currentColors.preview=e.target.value;updateStatus();});";
  html += "document.getElementById('off').addEventListener('input',e=>{currentColors.off=e.target.value;updateStatus();});";
  html += "document.getElementById('brightness').addEventListener('input',e=>{document.getElementById('bval').textContent=e.target.value;});";

  html += "document.getElementById('vmixForm').addEventListener('submit',e=>{";
  html += "e.preventDefault();";
  html += "const data={vmix_host:document.getElementById('host').value,vmix_port:parseInt(document.getElementById('port').value),vmix_input:document.getElementById('input').value,live_color:parseInt(currentColors.live.substring(1),16),preview_color:parseInt(currentColors.preview.substring(1),16),off_color:parseInt(currentColors.off.substring(1),16),brightness:parseInt(document.getElementById('brightness').value)};";
  html += "fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Configuration sauvegardee!');}).catch(e=>alert('Erreur: '+e));";
  html += "});";
  
  html += "document.getElementById('hardwareForm').addEventListener('submit',e=>{";
  html += "e.preventDefault();";
  html += "if(confirm('Sauvegarder la configuration matérielle? L\\'ESP32 va redémarrer.')){";
  html += "const data={led_pin:parseInt(document.getElementById('led_pin').value),led_count:parseInt(document.getElementById('led_count').value)};";
  html += "fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.json()).then(d=>{alert('Configuration sauvegardee! Redemarrage...');setTimeout(()=>location.reload(),3000);}).catch(e=>alert('Erreur: '+e));}";
  html += "});";

  html += "setInterval(updateStatus,1000);";
  html += "});";

  html += "function updateStatus(){";
  html += "fetch('/status').then(r=>r.json()).then(d=>{";
  html += "const header=document.getElementById('header'),badge=document.getElementById('statusBadge');";
  html += "document.getElementById('info').textContent='WiFi: '+(d.wifi_ssid||'AP Mode')+' | IP: '+(d.wifi_ip||'192.168.4.1');";
  html += "if(d.connected){";
  html += "if(d.live){";
  html += "header.className='header status-live';";
  html += "header.style.setProperty('--status-color',currentColors.live);";
  html += "badge.textContent='🔴 LIVE';";
  html += "}else if(d.preview){";
  html += "header.className='header status-preview';";
  html += "header.style.setProperty('--status-color',currentColors.preview);";
  html += "badge.textContent='🟢 PREVIEW';";
  html += "}else{";
  html += "header.className='header';";
  html += "header.style.removeProperty('--status-color');";
  html += "badge.textContent='✓ Connecté';";
  html += "}";
  html += "}else{";
  html += "header.className='header';";
  html += "header.style.removeProperty('--status-color');";
  html += "badge.textContent='⚠ Déconnecté';";
  html += "}";
  html += "}).catch(e=>console.error('Erreur status:',e));}";

  html += "function saveWiFi(){";
  html += "const ssid=document.getElementById('newssid').value,pwd=document.getElementById('pwd').value;";
  html += "if(!ssid){alert('Entrez un SSID');return;}";
  html += "fetch('/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:ssid,password:pwd})}).then(()=>{alert('WiFi configure! Redemarrage...');setTimeout(()=>location.reload(),3000)}).catch(e=>alert('Erreur: '+e));";
  html += "}";

  html += "function reboot(){if(confirm('Redemarrer?')){fetch('/reboot',{method:'POST'}).then(()=>alert('Redemarrage...'));}}";
  
  html += "function scanWiFi(){";
  html += "const list=document.getElementById('wifiList');";
  html += "list.innerHTML='<option value=\"\">Scan en cours...</option>';";
  html += "fetch('/scan').then(r=>r.json()).then(d=>{";
  html += "if(d.networks&&d.networks.length>0){";
  html += "list.innerHTML='<option value=\"\">-- Sélectionner un réseau --</option>';";
  html += "d.networks.forEach(n=>{";
  html += "const opt=document.createElement('option');";
  html += "opt.value=n.ssid;";
  html += "opt.textContent=n.ssid+' ('+n.rssi+' dBm)'+(n.secure?' 🔒':'');";
  html += "list.appendChild(opt);";
  html += "});";
  html += "}else{list.innerHTML='<option value=\"\">Aucun réseau trouvé</option>';}";
  html += "}).catch(e=>{list.innerHTML='<option value=\"\">Erreur de scan</option>';console.error(e);});";
  html += "}";
  
  html += "function selectNetwork(){";
  html += "const ssid=document.getElementById('wifiList').value;";
  html += "if(ssid)document.getElementById('newssid').value=ssid;";
  html += "}";
  
  html += "function switchTab(index){";
  html += "const tabs=document.querySelectorAll('.tab');";
  html += "const contents=document.querySelectorAll('.tab-content');";
  html += "tabs.forEach((t,i)=>{if(i===index){t.classList.add('active');}else{t.classList.remove('active');}});";
  html += "contents.forEach((c,i)=>{if(i===index){c.classList.add('active');}else{c.classList.remove('active');}});";
  html += "}";
  
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void handleConfig() {
  if (server.method() == HTTP_GET) {
    StaticJsonDocument<512> doc;
    doc["vmix_host"] = config.vmix_host;
    doc["vmix_port"] = config.vmix_port;
    doc["vmix_input"] = config.vmix_input;
    doc["live_color"] = config.live_color;
    doc["preview_color"] = config.preview_color;
    doc["off_color"] = config.off_color;
    doc["brightness"] = config.brightness;
    doc["led_pin"] = config.led_pin;
    doc["led_count"] = config.led_count;
    doc["wifi_ssid"] = WiFi.SSID();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
    
  } else if (server.method() == HTTP_POST) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    // deserializeJson retourne un DeserializationError qui peut être testé comme bool
    if (error) {
      LOG_ERROR("Erreur JSON: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
      return;
    }
    
    LOG_WEB("Réception configuration:");
    serializeJsonPretty(doc, Serial);
    Serial.println();
    
    Config updatedConfig = config;
    bool vmixConfigChanged = false;
    bool hardwareConfigChanged = false;
    
    // Ne mettre à jour que les paramètres présents dans le JSON
    if (doc.containsKey("vmix_host")) {
      String vmixHost = doc["vmix_host"] | "";
      vmixHost.trim();
      if (vmixHost.length() == 0 || vmixHost.length() >= sizeof(updatedConfig.vmix_host)) {
        server.send(400, "application/json", "{\"error\":\"Hôte VMix invalide\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_host, vmixHost.c_str(), sizeof(updatedConfig.vmix_host));
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_port")) {
      int vmixPort = doc["vmix_port"].as<int>();
      if (!isValidPort(vmixPort)) {
        server.send(400, "application/json", "{\"error\":\"Port VMix invalide\"}");
        return;
      }
      updatedConfig.vmix_port = vmixPort;
      vmixConfigChanged = true;
    }
    if (doc.containsKey("vmix_input")) {
      String vmixInput = doc["vmix_input"] | "";
      vmixInput.trim();
      if (!isValidVMixInput(vmixInput.c_str())) {
        server.send(400, "application/json", "{\"error\":\"Input VMix invalide\"}");
        return;
      }
      strlcpy(updatedConfig.vmix_input, vmixInput.c_str(), sizeof(updatedConfig.vmix_input));
      vmixConfigChanged = true;
    }
    
    // Les couleurs doivent être converties explicitement
    if (doc.containsKey("live_color")) {
      updatedConfig.live_color = doc["live_color"].as<unsigned long>() & 0xFFFFFF;
    }
    if (doc.containsKey("preview_color")) {
      updatedConfig.preview_color = doc["preview_color"].as<unsigned long>() & 0xFFFFFF;
    }
    if (doc.containsKey("off_color")) {
      updatedConfig.off_color = doc["off_color"].as<unsigned long>() & 0xFFFFFF;
    }
    
    if (doc.containsKey("brightness")) {
      int brightness = doc["brightness"].as<int>();
      if (!isValidBrightness(brightness)) {
        server.send(400, "application/json", "{\"error\":\"Luminosité invalide\"}");
        return;
      }
      updatedConfig.brightness = brightness;
    }
    if (doc.containsKey("led_pin")) {
      int ledPin = doc["led_pin"].as<int>();
      if (!isValidLedPin(ledPin)) {
        server.send(400, "application/json", "{\"error\":\"Pin LED invalide\"}");
        return;
      }
      updatedConfig.led_pin = ledPin;
      hardwareConfigChanged = true;
    }
    if (doc.containsKey("led_count")) {
      int ledCount = doc["led_count"].as<int>();
      if (!isValidLedCount(ledCount)) {
        server.send(400, "application/json", "{\"error\":\"Nombre de LEDs invalide\"}");
        return;
      }
      updatedConfig.led_count = ledCount;
      hardwareConfigChanged = true;
    }

    config = updatedConfig;
    
    // Log avant sauvegarde pour debug
    LOG_DEBUG("Valeurs avant sauvegarde:");
    LOG_DEBUG("  VMix: %s:%d Input:%s", config.vmix_host, config.vmix_port, config.vmix_input);
    LOG_DEBUG("  Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X Brightness=%d", 
              config.live_color, config.preview_color, config.off_color, config.brightness);
    LOG_DEBUG("  Matériel: Pin=%d Count=%d", config.led_pin, config.led_count);
    
    saveConfig();
    
    // Reconnecter VMix si configuration changée
    if (vmixConfigChanged) {
      if (vmixClient.connected()) {
        vmixClient.stop();
      }
      vmixConnected = false;
      setTally(false, false);
      lastVMixCheck = 0;
      LOG_VMIX("Configuration VMix changée, reconnexion...");
    }

    if (!hardwareConfigChanged) {
      applyTallyState();
    }
    
    // Redémarrer si config matérielle changée (LED pin/count)
    if (hardwareConfigChanged) {
      server.send(200, "application/json", "{\"status\":\"ok\",\"reboot\":true}");
      LOG_INFO("Configuration matérielle changée, redémarrage...");
      delay(500);
      ESP.restart();
    } else {
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    }
  }
}

void handleWiFi() {
  if (server.method() == HTTP_POST) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      LOG_ERROR("Erreur JSON WiFi: %s", error.c_str());
      server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
      return;
    }
    
    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";
    ssid.trim();
    
    if (ssid.length() > 0 && ssid.length() < sizeof(config.wifi_ssid) && password.length() < sizeof(config.wifi_password)) {
      LOG_WEB("Sauvegarde WiFi: %s", ssid.c_str());
      
      // Sauvegarder dans la config
      strlcpy(config.wifi_ssid, ssid.c_str(), sizeof(config.wifi_ssid));
      strlcpy(config.wifi_password, password.c_str(), sizeof(config.wifi_password));
      saveConfig();
      
      server.send(200, "application/json", "{\"status\":\"ok\"}");
      delay(500);
      ESP.restart();
    } else {
      server.send(400, "application/json", "{\"error\":\"SSID requis ou invalide\"}");
    }
  }
}

void handleStatus() {
  StaticJsonDocument<256> doc;
  doc["connected"] = vmixConnected;
  doc["live"] = isLive;
  doc["preview"] = isPreview;
  doc["vmix_host"] = config.vmix_host;
  doc["vmix_port"] = config.vmix_port;
  doc["vmix_input"] = config.vmix_input;
  doc["wifi_ssid"] = WiFi.SSID();
  doc["wifi_ip"] = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "";
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleScan() {
  LOG_WEB("Scan WiFi demandé...");
  int n = WiFi.scanNetworks();
  LOG_WEB("%d réseaux trouvés", n);
  
  // Utiliser ArduinoJson pour construction sécurisée
  int networkCount = n > 0 ? n : 0;
  size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(networkCount) + (networkCount * JSON_OBJECT_SIZE(3)) + 128;
  DynamicJsonDocument doc(capacity);
  JsonArray networks = doc.createNestedArray("networks");
  
  for (int i = 0; i < networkCount; i++) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
  }
  
  WiFi.scanDelete();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleReboot() {
  server.send(200, "application/json", "{\"status\":\"rebooting\"}");
  delay(500);
  ESP.restart();
}

#endif
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
  server.on("/vmix/inputs", handleVMixInputs);
  server.on("/status", handleStatus);
  server.on("/diagnostics", handleDiagnostics);
  server.on("/reboot", HTTP_POST, handleReboot);
  server.begin();
  
  LOG_INFO("\nSystème prêt!");
  LOG_INFO("=============================");
}

void loop() {
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
