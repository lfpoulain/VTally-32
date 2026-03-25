bool isValidPort(int port) {
  return port > 0 && port <= 65535;
}

bool isValidBrightness(int brightness) {
  return brightness >= 0 && brightness <= 255;
}

bool isValidLedPin(int ledPin) {
  return ledPin >= 0 && ledPin <= 48;
}

bool isValidLedCount(int ledCount) {
  return ledCount >= 1 && ledCount <= 255;
}

bool isValidVMixInput(const char* input) {
  size_t length = strlen(input);
  if (length == 0 || length >= sizeof(config.vmix_input)) {
    return false;
  }

  for (size_t i = 0; i < length; i++) {
    if (input[i] < '0' || input[i] > '9') {
      return false;
    }
  }

  return atoi(input) > 0;
}

bool isValidVMixKeyRefreshSeconds(int seconds) {
  return seconds >= 2 && seconds <= 3600;
}

void saveConfig() {
  LOG_INFO("Saving configuration...");
  preferences.begin("vtally32", false);

  preferences.putString("wifi_ssid", config.wifi_ssid);
  preferences.putString("wifi_pwd", config.wifi_password);
  preferences.putString("host", config.vmix_host);
  preferences.putInt("port", config.vmix_port);
  preferences.putString("input", config.vmix_input);
  preferences.putBool("track_key", config.vmix_track_by_key);
  preferences.putString("input_key", config.vmix_input_key);
  preferences.putString("input_title", config.vmix_input_title);
  preferences.putInt("key_refresh", config.vmix_key_refresh_seconds);
  preferences.putUInt("live", config.live_color);
  preferences.putUInt("preview", config.preview_color);
  preferences.putUInt("off", config.off_color);
  preferences.putInt("brightness", config.brightness);
  preferences.putInt("led_pin", config.led_pin);
  preferences.putInt("led_count", config.led_count);

  preferences.end();

  LOG_DEBUG("Colors: Live=0x%06X Preview=0x%06X Off=0x%06X",
            config.live_color, config.preview_color, config.off_color);
  LOG_DEBUG("VMix API: %s:%d TCP TALLY:%d Input:%s TrackByKey:%s Refresh:%ds",
            config.vmix_host,
            config.vmix_port,
            VMIX_TCP_PORT,
            config.vmix_input,
            config.vmix_track_by_key ? "YES" : "NO",
            config.vmix_key_refresh_seconds);
  LOG_DEBUG("LED: Pin=%d Count=%d Brightness=%d",
            config.led_pin, config.led_count, config.brightness);
  LOG_DEBUG("WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Not configured");
  LOG_INFO("Config saved");
}

void loadConfig() {
  preferences.begin("vtally32", true);

  strlcpy(config.wifi_ssid, preferences.getString("wifi_ssid", "").c_str(), sizeof(config.wifi_ssid));
  strlcpy(config.wifi_password, preferences.getString("wifi_pwd", "").c_str(), sizeof(config.wifi_password));
  strlcpy(config.vmix_host, preferences.getString("host", DEFAULT_VMIX_HOST).c_str(), sizeof(config.vmix_host));
  config.vmix_port = preferences.getInt("port", DEFAULT_VMIX_PORT);
  strlcpy(config.vmix_input, preferences.getString("input", DEFAULT_VMIX_INPUT).c_str(), sizeof(config.vmix_input));
  config.vmix_track_by_key = preferences.getBool("track_key", DEFAULT_VMIX_TRACK_BY_KEY);
  strlcpy(config.vmix_input_key, preferences.getString("input_key", "").c_str(), sizeof(config.vmix_input_key));
  strlcpy(config.vmix_input_title, preferences.getString("input_title", "").c_str(), sizeof(config.vmix_input_title));
  config.vmix_key_refresh_seconds = preferences.getInt("key_refresh", DEFAULT_VMIX_KEY_REFRESH_SECONDS);
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
  if (!isValidVMixKeyRefreshSeconds(config.vmix_key_refresh_seconds)) {
    config.vmix_key_refresh_seconds = DEFAULT_VMIX_KEY_REFRESH_SECONDS;
  }
  if (strlen(config.vmix_input_key) >= sizeof(config.vmix_input_key)) {
    config.vmix_input_key[0] = '\0';
  }
  if (strlen(config.vmix_input_title) >= sizeof(config.vmix_input_title)) {
    config.vmix_input_title[0] = '\0';
  }
  if (config.vmix_track_by_key && strlen(config.vmix_input_key) == 0) {
    config.vmix_track_by_key = false;
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

  LOG_INFO("Config loaded:");
  LOG_DEBUG("  WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Not configured");
  LOG_DEBUG("  VMix API: %s:%d TCP TALLY:%d Input:%s TrackByKey:%s Refresh:%ds",
            config.vmix_host,
            config.vmix_port,
            VMIX_TCP_PORT,
            config.vmix_input,
            config.vmix_track_by_key ? "YES" : "NO",
            config.vmix_key_refresh_seconds);
  LOG_DEBUG("  Colors: Live=0x%06X Preview=0x%06X Off=0x%06X",
            config.live_color, config.preview_color, config.off_color);
}
