bool connectVMix() {
  if (vmixClient.connected()) {
    LOG_VMIX("Déjà connecté à VMix");
    vmixCheckInterval = VMIX_CHECK_INTERVAL; // Reset backoff
    return true;
  }

  // Fermeture préventive du socket pour éviter les fuites de mémoire (Memory Leak)
  vmixClient.stop();

  // Le port TCP vMix est fixé à 8099 pour l'API de tally
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
        vmixClient.stop();
        return false;
      }

      delay(10);
    }

    if (success) {
      vmixConnected = true;
      vmixCheckInterval = VMIX_CHECK_INTERVAL; // Reset backoff upon success
      LOG_VMIX("VMix connecté et abonné avec succès");
      return true;
    }

    LOG_ERROR("VMix n'a pas répondu - déconnexion");
    vmixClient.stop();
    return false;
  }

  LOG_ERROR("Échec connexion VMix");
  vmixClient.stop(); // Force release socket

  // Exponential backoff up to 30 seconds
  vmixCheckInterval = min((unsigned long)30000, vmixCheckInterval * 2);
  LOG_VMIX("Prochaine tentative dans %lu ms", vmixCheckInterval);

  return false;
}

void checkVMix() {
  // Déconnexion immédiate si le socket est fermé
  if (vmixConnected && !vmixClient.connected()) {
    LOG_VMIX("Connexion VMix perdue");
    vmixConnected = false;
    vmixClient.stop();
    setTally(false, false);
  }

  // Tentative de reconnexion périodique avec backoff exponentiel
  if (millis() - lastVMixCheck < vmixCheckInterval) return;
  lastVMixCheck = millis();

  if (!vmixConnected) {
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
