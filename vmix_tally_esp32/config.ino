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

void saveConfig() {
  LOG_INFO("Sauvegarde configuration...");
  preferences.begin("vtally32", false);

  preferences.putString("tally_name", config.tally_name);
  preferences.putString("wifi_ssid", config.wifi_ssid);
  preferences.putString("wifi_pwd", config.wifi_password);
  preferences.putString("host", config.vmix_host);
  preferences.putString("input", config.vmix_input);
  preferences.putUInt("live", config.live_color);
  preferences.putUInt("preview", config.preview_color);
  preferences.putUInt("off", config.off_color);
  preferences.putInt("brightness", config.brightness);
  preferences.putInt("led_pin", config.led_pin);
  preferences.putInt("led_count", config.led_count);

  preferences.end();

  LOG_DEBUG("Nom: %s", config.tally_name);
  LOG_DEBUG("Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X",
            config.live_color, config.preview_color, config.off_color);
  LOG_DEBUG("VMix: %s:8099 Input:%s",
            config.vmix_host, config.vmix_input);
  LOG_DEBUG("LED: Pin=%d Count=%d Brightness=%d",
            config.led_pin, config.led_count, config.brightness);
  LOG_DEBUG("WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Non configuré");
  LOG_INFO("Configuration sauvegardée");
}

void loadConfig() {
  preferences.begin("vtally32", true);

  strlcpy(config.tally_name, preferences.getString("tally_name", DEFAULT_TALLY_NAME).c_str(), sizeof(config.tally_name));
  strlcpy(config.wifi_ssid, preferences.getString("wifi_ssid", "").c_str(), sizeof(config.wifi_ssid));
  strlcpy(config.wifi_password, preferences.getString("wifi_pwd", "").c_str(), sizeof(config.wifi_password));
  strlcpy(config.vmix_host, preferences.getString("host", DEFAULT_VMIX_HOST).c_str(), sizeof(config.vmix_host));
  strlcpy(config.vmix_input, preferences.getString("input", DEFAULT_VMIX_INPUT).c_str(), sizeof(config.vmix_input));
  config.live_color = preferences.getUInt("live", DEFAULT_LIVE_COLOR);
  config.preview_color = preferences.getUInt("preview", DEFAULT_PREVIEW_COLOR);
  config.off_color = preferences.getUInt("off", DEFAULT_OFF_COLOR);
  config.brightness = preferences.getInt("brightness", DEFAULT_BRIGHTNESS);
  config.led_pin = preferences.getInt("led_pin", DEFAULT_LED_PIN);
  config.led_count = preferences.getInt("led_count", DEFAULT_LED_COUNT);

  if (strlen(config.tally_name) == 0) {
    strlcpy(config.tally_name, DEFAULT_TALLY_NAME, sizeof(config.tally_name));
  }
  if (strlen(config.vmix_host) == 0) {
    strlcpy(config.vmix_host, DEFAULT_VMIX_HOST, sizeof(config.vmix_host));
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
  LOG_DEBUG("  Nom: %s", config.tally_name);
  LOG_DEBUG("  WiFi: %s", strlen(config.wifi_ssid) > 0 ? config.wifi_ssid : "Non configuré");
  LOG_DEBUG("  VMix: %s:8099 Input:%s", config.vmix_host, config.vmix_input);
  LOG_DEBUG("  Couleurs: Live=0x%06X Preview=0x%06X Off=0x%06X",
            config.live_color, config.preview_color, config.off_color);
}
