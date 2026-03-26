String getWiFiModeLabel() {
  wifi_mode_t mode = WiFi.getMode();

  switch (mode) {
    case WIFI_OFF:
      return "OFF";
    case WIFI_STA:
      return "STA";
    case WIFI_AP:
      return "AP";
    case WIFI_AP_STA:
      return "AP+STA";
    default:
      return "UNKNOWN";
  }
}

String formatUptime(unsigned long uptimeMs) {
  unsigned long totalSeconds = uptimeMs / 1000;
  unsigned long days = totalSeconds / 86400;
  unsigned long hours = (totalSeconds % 86400) / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%lud %02luh %02lum %02lus", days, hours, minutes, seconds);
  return String(buffer);
}

void handleDiagnostics() {
  StaticJsonDocument<768> doc;
  bool staConnected = WiFi.status() == WL_CONNECTED;
  String staIp = staConnected ? WiFi.localIP().toString() : "";
  String apIp = WiFi.softAPIP().toString();

  doc["firmware_version"] = "2.0.0";
  doc["tally_name"] = config.tally_name;
  doc["uptime_ms"] = millis();
  doc["uptime_human"] = formatUptime(millis());
  doc["free_heap"] = ESP.getFreeHeap();
  doc["min_free_heap"] = ESP.getMinFreeHeap();
  doc["max_alloc_heap"] = ESP.getMaxAllocHeap();
  doc["wifi_mode"] = getWiFiModeLabel();
  doc["ap_active"] = apActive;
  doc["sta_connected"] = staConnected;
  doc["wifi_ssid"] = staConnected ? WiFi.SSID() : "";
  doc["wifi_ip"] = staIp;
  doc["ap_ip"] = apIp;
  doc["wifi_rssi"] = staConnected ? WiFi.RSSI() : 0;
  doc["vmix_connected"] = vmixConnected;
  doc["vmix_socket_connected"] = vmixClient.connected();
  doc["vmix_host"] = config.vmix_host;
  doc["vmix_port"] = 8099;
  doc["vmix_input"] = config.vmix_input;
  doc["tally_state"] = lastState;
  doc["live"] = isLive;
  doc["preview"] = isPreview;
  doc["led_pin"] = config.led_pin;
  doc["led_count"] = config.led_count;
  doc["brightness"] = config.brightness;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}
