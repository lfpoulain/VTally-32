void startAP() {
  WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
  apActive = true;
  LOG_NETWORK("AP started: http://%s", WiFi.softAPIP().toString().c_str());
}

void setupWiFi() {
  LOG_NETWORK("Setting up WiFi...");
  WiFi.mode(WIFI_AP_STA);

  startAP();

  if (strlen(config.wifi_ssid) > 0) {
    bool connected = false;

    for (int attempt = 1; attempt <= WIFI_RETRY_COUNT && !connected; attempt++) {
      LOG_NETWORK("Attempt %d/%d - Connecting to: %s", attempt, WIFI_RETRY_COUNT, config.wifi_ssid);
      WiFi.begin(config.wifi_ssid, config.wifi_password);

      unsigned long start = millis();
      while (WiFi.status() != WL_CONNECTED && (millis() - start) < (WIFI_RETRY_TIMEOUT * 1000)) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();

      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        LOG_NETWORK("WiFi connected: %s (%s)", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        apActive = false;
        LOG_NETWORK("AP disabled (WiFi connected)");
      } else {
        LOG_WARN("Attempt %d/%d failed", attempt, WIFI_RETRY_COUNT);
        if (attempt < WIFI_RETRY_COUNT) {
          WiFi.disconnect();
          delay(1000);
        }
      }
    }

    if (!connected) {
      LOG_ERROR("WiFi connection failed after %d attempts - AP mode only", WIFI_RETRY_COUNT);
    }
  } else {
    LOG_INFO("No WiFi configured - AP mode only");
  }
}

void checkWiFi() {
  if (millis() - lastWiFiCheck < WIFI_CHECK_INTERVAL) return;
  lastWiFiCheck = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if (apActive) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      apActive = false;
      LOG_NETWORK("AP disabled (WiFi reconnected)");
    }
  } else {
    if (strlen(config.wifi_ssid) > 0) {
      LOG_NETWORK("Trying WiFi reconnection...");
      WiFi.begin(config.wifi_ssid, config.wifi_password);
    }

    if (vmixClient.connected() || vmixConnected) {
      vmixClient.stop();
      vmixConnected = false;
      setTally(false, false);
    }

    if (!apActive) {
      WiFi.mode(WIFI_AP_STA);
      startAP();
      LOG_NETWORK("AP re-enabled (WiFi lost)");
    }
  }
}
