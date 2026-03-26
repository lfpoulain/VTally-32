void startAP() {
  WiFi.softAP(config.tally_name, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
  apActive = true;
  LOG_NETWORK("AP démarré: %s - IP: http://%s", config.tally_name, WiFi.softAPIP().toString().c_str());
}

String sanitizeHostname(const char* sourceName) {
  String sanitized;
  bool lastWasHyphen = false;

  for (size_t i = 0; sourceName[i] != '\0'; i++) {
    char c = sourceName[i];

    if (c >= 'A' && c <= 'Z') {
      c = c - 'A' + 'a';
    }

    bool isAlphaNum = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
    bool isSeparator = (c == ' ' || c == '-' || c == '_' || c == '.');

    if (isAlphaNum) {
      sanitized += c;
      lastWasHyphen = false;
    } else if (isSeparator && !lastWasHyphen && sanitized.length() > 0) {
      sanitized += '-';
      lastWasHyphen = true;
    }

    if (sanitized.length() >= 31) {
      break;
    }
  }

  while (sanitized.endsWith("-")) {
    sanitized.remove(sanitized.length() - 1);
  }

  if (sanitized.length() == 0) {
    sanitized = "vtally32";
  }

  return sanitized;
}

void setupWiFi() {
  LOG_NETWORK("Configuration WiFi en cours...");

  WiFi.mode(WIFI_AP_STA);

  String networkHostname = sanitizeHostname(config.tally_name);
  WiFi.setHostname(networkHostname.c_str());
  LOG_NETWORK("Nom réseau: %s (affiché: %s)", networkHostname.c_str(), config.tally_name);

  startAP();

  if (strlen(config.wifi_ssid) > 0) {
    bool connected = false;

    for (int attempt = 1; attempt <= WIFI_RETRY_COUNT && !connected; attempt++) {
      LOG_NETWORK("Tentative %d/%d - Connexion à: %s", attempt, WIFI_RETRY_COUNT, config.wifi_ssid);
      WiFi.setHostname(networkHostname.c_str());
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

  // Démarrage mDNS pour accès via nom.local
  if (MDNS.begin(networkHostname.c_str())) {
    LOG_NETWORK("mDNS démarré. Accès via http://%s.local", networkHostname.c_str());
    MDNS.addService("http", "tcp", 80);
  } else {
    LOG_WARN("Échec démarrage mDNS avec le nom %s", networkHostname.c_str());
  }
}

void checkWiFi() {
  bool currentConnected = (WiFi.status() == WL_CONNECTED);

  // Déconnexion immédiate
  if (!currentConnected) {
    if (vmixClient.connected() || vmixConnected) {
      vmixClient.stop();
      vmixConnected = false;
      setTally(false, false);
      LOG_NETWORK("WiFi perdu - arrêt du Tally");
    }
  }

  // Vérifications et reconnexions périodiques
  if (millis() - lastWiFiCheck < WIFI_CHECK_INTERVAL) return;
  lastWiFiCheck = millis();

  if (currentConnected) {
    if (apActive) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      apActive = false;
      LOG_NETWORK("AP désactivé (WiFi reconnecté)");
    }
  } else {
    if (strlen(config.wifi_ssid) > 0) {
      LOG_NETWORK("Tentative reconnexion WiFi...");
      String networkHostname = sanitizeHostname(config.tally_name);
      WiFi.setHostname(networkHostname.c_str());
      WiFi.begin(config.wifi_ssid, config.wifi_password);
    }

    if (!apActive) {
      WiFi.mode(WIFI_AP_STA);
      startAP();
      LOG_NETWORK("AP réactivé (WiFi perdu)");
    }
  }
}
