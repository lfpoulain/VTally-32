void resetVMixConnectionState() {
  vmixClient.stop();
  vmixConnected = false;
  vmixLineBuffer = "";
  vmixCheckInterval = VMIX_CHECK_INTERVAL;
  lastVMixCheck = 0;
  setTally(false, false);
}

void requestVMixReconnect() {
  LOG_VMIX("Reconnexion VMix forcée");
  resetVMixConnectionState();
  setDebugStage(2, "VMIX_RECONNECT_QUEUED");
}

bool connectVMix() {
  if (vmixClient.connected()) {
    LOG_VMIX("Déjà connecté à VMix");
    vmixCheckInterval = VMIX_CHECK_INTERVAL;
    return true;
  }

  if (WiFi.status() != WL_CONNECTED) {
    setDebugStage(1, "WIFI_CONNECTING");
    return false;
  }

  vmixClient.stop();
  setDebugStage(2, "VMIX_CONNECTING");

  LOG_VMIX("Connexion à VMix %s:8099...", config.vmix_host);

  vmixClient.setNoDelay(true);
  vmixClient.setTimeout(VMIX_TCP_TIMEOUT);

  if (vmixClient.connect(config.vmix_host, 8099)) {
    delay(100);

    if (!vmixClient.connected()) {
      LOG_ERROR("Connexion fermée par VMix");
      vmixClient.stop();
      return false;
    }

    vmixClient.print("SUBSCRIBE TALLY\r\n");
    vmixClient.flush();

    unsigned long start = millis();
    bool success = false;
    String responseBuffer = "";

    while (millis() - start < VMIX_RESPONSE_TIMEOUT && !success) {
      while (vmixClient.available()) {
        char c = (char)vmixClient.read();
        if (c == '\r') {
          continue;
        }
        if (c == '\n') {
          String response = responseBuffer;
          responseBuffer = "";
          response.trim();

          if (response.startsWith("SUBSCRIBE OK") || response.startsWith("TALLY OK")) {
            success = true;
            LOG_VMIX("Réponse reçue: %s", response.c_str());
            if (response.startsWith("TALLY OK")) {
              parseVMix(response);
            }
          }
        } else {
          responseBuffer += c;
          if (responseBuffer.length() > 256) {
            responseBuffer.remove(0, responseBuffer.length() - 128);
          }
        }
      }

      if (!vmixClient.connected()) {
        LOG_VMIX("VMix a fermé la connexion");
        vmixClient.stop();
        return false;
      }

      delay(10);
    }

    if (success) {
      vmixConnected = true;
      vmixCheckInterval = VMIX_CHECK_INTERVAL;
      vmixLineBuffer = "";
      setDebugStage(3, "VMIX_CONNECTED");
      LOG_VMIX("VMix connecté et abonné avec succès");
      return true;
    }

    LOG_ERROR("VMix n'a pas répondu - déconnexion");
    vmixClient.stop();
    setDebugStage(6, "VMIX_NO_RESPONSE");
    return false;
  }

  LOG_ERROR("Échec connexion VMix");
  vmixClient.stop();
  setDebugStage(6, "VMIX_CONNECT_FAILED");

  vmixCheckInterval = min((unsigned long)VMIX_MAX_RETRY_INTERVAL, vmixCheckInterval * 2);
  LOG_VMIX("Prochaine tentative dans %lu ms", vmixCheckInterval);

  return false;
}

void checkVMix() {
  if (vmixConnected && !vmixClient.connected()) {
    LOG_VMIX("Connexion VMix perdue");
    vmixConnected = false;
    vmixClient.stop();
    vmixLineBuffer = "";
    setTally(false, false);
    setDebugStage(6, "VMIX_DISCONNECTED");
  }

  if (millis() - lastVMixCheck < vmixCheckInterval) return;
  lastVMixCheck = millis();

  if (!vmixConnected) {
    connectVMix();
  }
}

void handleVMix() {
  while (vmixClient.connected() && vmixClient.available()) {
    char c = (char)vmixClient.read();
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      String line = vmixLineBuffer;
      vmixLineBuffer = "";
      line.trim();
      if (line.length() > 0) {
        LOG_DEBUG("VMix reçu: %s", line.c_str());
        parseVMix(line);
      }
    } else {
      vmixLineBuffer += c;
      if (vmixLineBuffer.length() > 256) {
        vmixLineBuffer.remove(0, vmixLineBuffer.length() - 128);
      }
    }
  }
}

void parseVMix(const String& cmd) {
  if (!cmd.startsWith("TALLY OK")) return;
  if (cmd.length() < 10) return;

  String tallyData = cmd.substring(9);
  tallyData.trim();

  if (strlen(config.vmix_input) == 0) {
    LOG_WARN("Input VMix non configuré");
    return;
  }

  int inputIndex = atoi(config.vmix_input) - 1;

  if (inputIndex < 0) {
    LOG_WARN("Input VMix invalide: %s", config.vmix_input);
    return;
  }

  if (inputIndex < tallyData.length()) {
    char state = tallyData.charAt(inputIndex);
    bool live = (state == '1' || state == '3');
    bool preview = (state == '2' || state == '3');

    setTally(live, preview);
  } else {
    LOG_WARN("Input %s hors limites (max: %d)", config.vmix_input, tallyData.length());
  }
}
