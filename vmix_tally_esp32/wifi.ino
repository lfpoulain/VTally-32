void startAP() {
  WiFi.softAP(config.tally_name, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
  apActive = true;
  LOG_NETWORK("AP démarré: %s - IP: http://%s", config.tally_name, WiFi.softAPIP().toString().c_str());
}

void setDebugStage(uint8_t code, const char* label) {
  debugStageCode = code;
  debugStageLabel = label;
  applyTallyState();
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
  setDebugStage(1, "WIFI_CONNECTING");

  // Pas d'écriture NVS à chaque begin(), reconnexion gérée par le driver,
  // et pas de modem sleep (latence et pertes de paquets). setSleep() est
  // mémorisé par le core et réappliqué à chaque démarrage du STA.
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
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
      WiFi.mode(WIFI_AP_STA);
      WiFi.disconnect();
      delay(100);
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
        setDebugStage(8, "WIFI_READY");

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
      setDebugStage(9, "WIFI_FAILED");
      LOG_ERROR("Échec connexion WiFi après %d tentatives - Mode AP uniquement", WIFI_RETRY_COUNT);
    }
  } else {
    setDebugStage(7, "AP_ONLY");
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
  if (millis() - lastWiFiCheck < 500) return;
  lastWiFiCheck = millis();

  bool currentConnected = (getWiFiStatusCached() == WL_CONNECTED);
  unsigned long now = millis();

  // WiFi connecté : maintenance
  if (currentConnected) {
    if (wifiLostSince != 0) {
      LOG_NETWORK("WiFi rétabli après %lu ms", now - wifiLostSince);
      wifiLostSince = 0;
    }
    if (debugStageCode == 1 || debugStageCode == 9) {
      setDebugStage(8, "WIFI_READY");
    }
    if (apActive) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      apActive = false;
      LOG_NETWORK("AP désactivé (WiFi reconnecté)");
    }
    return;
  }

  // Pas de WiFi configuré : l'AP démarré au boot reste actif
  if (strlen(config.wifi_ssid) == 0) {
    return;
  }

  // WiFi perdu : on laisse l'auto-reconnect du driver agir. La connexion VMix
  // n'est pas coupée ici, une micro-coupure peut la laisser intacte ; le
  // watchdog de checkVMix() éteint le tally si VMix ne répond plus.
  if (wifiLostSince == 0) {
    wifiLostSince = now;
    lastWiFiReconnectAttempt = now;
    setDebugStage(1, "WIFI_CONNECTING");
    LOG_NETWORK("WiFi perdu - reconnexion automatique en cours");
    return;
  }

  if (now - lastWiFiReconnectAttempt >= WIFI_RECONNECT_INTERVAL) {
    lastWiFiReconnectAttempt = now;
    LOG_NETWORK("Relance de la connexion WiFi vers %s", config.wifi_ssid);
    WiFi.reconnect();
  }

  if (!apActive && now - wifiLostSince >= WIFI_AP_FALLBACK_DELAY) {
    WiFi.mode(WIFI_AP_STA);
    startAP();
    setDebugStage(9, "WIFI_FAILED");
    LOG_NETWORK("AP réactivé (WiFi perdu depuis %lu s)", (now - wifiLostSince) / 1000);
  }
}
