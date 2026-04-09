void resetVMixConnectionState() {
  vmixClient.stop();
  vmixConnected = false;
  vmixLineBufferLen = 0;
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

  if (getWiFiStatusCached() != WL_CONNECTED) {
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
    char responseBuf[260];
    int responseBufLen = 0;

    while (millis() - start < VMIX_RESPONSE_TIMEOUT && !success) {
      while (vmixClient.available()) {
        char c = (char)vmixClient.read();
        if (c == '\r') continue;
        if (c == '\n') {
          responseBuf[responseBufLen] = '\0';
          if (responseBufLen > 0) {
            if (strncmp(responseBuf, "SUBSCRIBE OK", 12) == 0 || strncmp(responseBuf, "TALLY OK", 8) == 0) {
              success = true;
              LOG_VMIX("Réponse reçue: %s", responseBuf);
              if (strncmp(responseBuf, "TALLY OK", 8) == 0) {
                parseVMix(responseBuf);
              }
            }
          }
          responseBufLen = 0;
        } else {
          if (responseBufLen < (int)(sizeof(responseBuf) - 1)) {
            responseBuf[responseBufLen++] = c;
          }
        }
      }

      if (!vmixClient.connected()) {
        LOG_VMIX("VMix a fermé la connexion");
        vmixClient.stop();
        return false;
      }

      delay(1);
    }

    if (success) {
      // TCP Keepalive : détecter une connexion morte en ~35s au lieu de 2min+
      int fd = vmixClient.fd();
      if (fd >= 0) {
        int keepAlive = 1;
        int keepIdle = 5;       // Première sonde après 5s d'inactivité
        int keepInterval = 5;   // Sondes toutes les 5s
        int keepCount = 6;      // Abandon après 6 échecs = ~35s total
        setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle));
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval));
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount));
        LOG_VMIX("TCP keepalive activé (détection ~35s)");
      }

      vmixConnected = true;
      vmixCheckInterval = VMIX_CHECK_INTERVAL;
      vmixLineBufferLen = 0;
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
    vmixLineBufferLen = 0;
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
    if (c == '\r') continue;
    if (c == '\n') {
      vmixLineBuffer[vmixLineBufferLen] = '\0';
      if (vmixLineBufferLen > 0) {
        LOG_DEBUG("VMix reçu: %s", vmixLineBuffer);
        parseVMix(vmixLineBuffer);
      }
      vmixLineBufferLen = 0;
    } else {
      if (vmixLineBufferLen < (int)(sizeof(vmixLineBuffer) - 1)) {
        vmixLineBuffer[vmixLineBufferLen++] = c;
      }
    }
  }
}

void parseVMix(const char* cmd) {
  if (strncmp(cmd, "TALLY OK", 8) != 0) return;
  int cmdLen = strlen(cmd);
  if (cmdLen < 10) return;

  const char* tallyData = cmd + 9;
  while (*tallyData == ' ') tallyData++;

  if (strlen(config.vmix_input) == 0) {
    LOG_WARN("Input VMix non configuré");
    return;
  }

  int inputIndex = atoi(config.vmix_input) - 1;

  if (inputIndex < 0) {
    LOG_WARN("Input VMix invalide: %s", config.vmix_input);
    return;
  }

  int tallyLen = strlen(tallyData);
  if (inputIndex < tallyLen) {
    char state = tallyData[inputIndex];
    bool live = (state == '1' || state == '3');
    bool preview = (state == '2' || state == '3');
    setTally(live, preview);
  } else {
    LOG_WARN("Input %s hors limites (max: %d)", config.vmix_input, tallyLen);
  }
}
