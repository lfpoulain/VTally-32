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

bool failVMixConnect(const char* stage) {
  vmixClient.stop();
  setDebugStage(6, stage);

  vmixCheckInterval = min((unsigned long)VMIX_MAX_RETRY_INTERVAL, vmixCheckInterval * 2);
  LOG_VMIX("Prochaine tentative dans %lu ms", vmixCheckInterval);

  return false;
}

bool connectVMix() {
  if (getWiFiStatusCached() != WL_CONNECTED) {
    setDebugStage(1, "WIFI_CONNECTING");
    return false;
  }

  vmixClient.stop();
  setDebugStage(2, "VMIX_CONNECTING");

  LOG_VMIX("Connexion à VMix %s:8099...", config.vmix_host);

  if (!vmixClient.connect(config.vmix_host, 8099, VMIX_CONNECT_TIMEOUT)) {
    LOG_ERROR("Échec connexion VMix");
    return failVMixConnect("VMIX_CONNECT_FAILED");
  }

  // Les options socket ne s'appliquent qu'une fois le socket créé
  vmixClient.setNoDelay(true);

  vmixClient.print("SUBSCRIBE TALLY\r\n");

  unsigned long start = millis();
  bool subscribed = false;
  char responseBuf[VMIX_MAX_LINE_LENGTH];
  int responseBufLen = 0;

  while (millis() - start < VMIX_RESPONSE_TIMEOUT && !subscribed) {
    // Arrêt dès SUBSCRIBE OK : les octets suivants restent dans le socket pour handleVMix()
    while (!subscribed && vmixClient.available()) {
      char c = (char)vmixClient.read();
      if (c == '\r') continue;
      if (c == '\n') {
        responseBuf[responseBufLen] = '\0';
        if (responseBufLen > 0) {
          if (strncmp(responseBuf, "SUBSCRIBE OK", 12) == 0) {
            subscribed = true;
            LOG_VMIX("Réponse reçue: %s", responseBuf);
          } else if (strncmp(responseBuf, "TALLY OK", 8) == 0) {
            parseVMix(responseBuf);
          } else if (strncmp(responseBuf, "SUBSCRIBE ER", 12) == 0) {
            LOG_ERROR("Abonnement TALLY refuse: %s", responseBuf);
            return failVMixConnect("VMIX_SUBSCRIBE_FAILED");
          }
        }
        responseBufLen = 0;
      } else {
        if (responseBufLen < (int)(sizeof(responseBuf) - 1)) {
          responseBuf[responseBufLen++] = c;
        }
      }
    }

    if (!subscribed && !vmixClient.connected()) {
      LOG_VMIX("VMix a fermé la connexion");
      return failVMixConnect("VMIX_DISCONNECTED");
    }

    delay(1);
  }

  if (!subscribed) {
    LOG_ERROR("VMix n'a pas répondu - déconnexion");
    return failVMixConnect("VMIX_NO_RESPONSE");
  }

  // TCP Keepalive : filet de sécurité en plus du heartbeat applicatif
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
  }

  // Demande immédiate de l'état courant
  vmixClient.print("TALLY\r\n");

  vmixConnected = true;
  vmixCheckInterval = VMIX_CHECK_INTERVAL;
  vmixLineBufferLen = 0;
  lastVMixRx = millis();
  lastVMixHeartbeat = millis();
  setDebugStage(3, "VMIX_CONNECTED");
  LOG_VMIX("VMix connecté et abonné avec succès");
  return true;
}

void dropVMixConnection(const char* reason) {
  LOG_VMIX("Connexion VMix perdue (%s)", reason);
  vmixConnected = false;
  vmixClient.stop();
  vmixLineBufferLen = 0;
  setTally(false, false);
  setDebugStage(6, "VMIX_DISCONNECTED");
}

void checkVMix() {
  unsigned long now = millis();

  if (vmixConnected) {
    if (!vmixClient.connected()) {
      dropVMixConnection("socket fermé");
    } else if (now - lastVMixRx > VMIX_RX_TIMEOUT) {
      // VMix répond toujours à TALLY : un silence prolongé signifie une connexion morte
      dropVMixConnection("aucune réponse au heartbeat");
    } else {
      if (now - lastVMixHeartbeat >= VMIX_HEARTBEAT_INTERVAL) {
        lastVMixHeartbeat = now;
        vmixClient.print("TALLY\r\n");
      }
      return;
    }
  }

  if (now - lastVMixCheck < vmixCheckInterval) return;
  lastVMixCheck = now;

  connectVMix();
}

void handleVMix() {
  while (vmixClient.available()) {
    char c = (char)vmixClient.read();
    if (c == '\r') continue;
    if (c == '\n') {
      vmixLineBuffer[vmixLineBufferLen] = '\0';
      if (vmixLineBufferLen > 0) {
        lastVMixRx = millis();
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
