bool connectVMix() {
  if (vmixClient.connected()) {
    LOG_VMIX("Already connected to vMix");
    return true;
  }

  LOG_VMIX("Connecting TCP TALLY to vMix %s:%d...", config.vmix_host, VMIX_TCP_PORT);

  vmixClient.setNoDelay(true);
  vmixClient.setTimeout(VMIX_TCP_TIMEOUT);

  if (vmixClient.connect(config.vmix_host, VMIX_TCP_PORT)) {
    delay(100);

    if (!vmixClient.connected()) {
      LOG_ERROR("Connection closed by vMix");
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
          LOG_VMIX("Response received: %s", response.c_str());
          if (response.startsWith("TALLY OK")) {
            parseVMix(response);
          }
        }
      }

      if (!vmixClient.connected()) {
        LOG_VMIX("vMix closed the connection");
        return false;
      }

      delay(10);
    }

    if (success) {
      vmixConnected = true;
      refreshResolvedVMixInput(true);
      LOG_VMIX("vMix connected and subscribed successfully");
      return true;
    }

    LOG_ERROR("vMix did not respond - disconnecting");
    vmixClient.stop();
    return false;
  }

  LOG_ERROR("Failed to connect to vMix");
  return false;
}

void checkVMix() {
  if (vmixClient.connected() && config.vmix_track_by_key && WiFi.status() == WL_CONNECTED) {
    refreshResolvedVMixInput(false);
  }

  if (millis() - lastVMixCheck < VMIX_CHECK_INTERVAL) return;
  lastVMixCheck = millis();

  if (!vmixClient.connected()) {
    if (vmixConnected) {
      LOG_VMIX("vMix connection lost");
      vmixConnected = false;
      setTally(false, false);
    }
    connectVMix();
  } else if (config.vmix_track_by_key && WiFi.status() == WL_CONNECTED) {
    refreshResolvedVMixInput(false);
  }
}

void handleVMix() {
  while (vmixClient.available()) {
    String line = vmixClient.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      LOG_DEBUG("vMix received: %s", line.c_str());
      parseVMix(line);
    }
  }
}

String decodeXmlEntities(const String& value) {
  String decoded = value;
  decoded.replace("&amp;", "&");
  decoded.replace("&quot;", "\"");
  decoded.replace("&apos;", "'");
  decoded.replace("&lt;", "<");
  decoded.replace("&gt;", ">");
  return decoded;
}

String extractXmlAttribute(const String& tag, const char* attributeName) {
  String pattern = String(attributeName) + "=\"";
  int start = tag.indexOf(pattern);

  if (start < 0) {
    return "";
  }

  start += pattern.length();
  int end = tag.indexOf('"', start);

  if (end < 0) {
    return "";
  }

  return decodeXmlEntities(tag.substring(start, end));
}

int countVMixInputsInXml(const String& xml) {
  int count = 0;
  int searchIndex = 0;

  while (true) {
    int tagStart = xml.indexOf("<input ", searchIndex);

    if (tagStart < 0) {
      break;
    }

    count++;
    searchIndex = tagStart + 7;
  }

  return count;
}

bool fetchVMixApiXml(const char* host, int port, String& xml, String& errorMessage) {
  WiFiClient apiClient;
  apiClient.setTimeout(VMIX_RESPONSE_TIMEOUT);

  if (!apiClient.connect(host, port)) {
    errorMessage = "Unable to connect to the vMix API";
    return false;
  }

  apiClient.printf("GET /API/? HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: VTally-32\r\n\r\n", host);

  String response;
  response.reserve(16384);

  unsigned long deadline = millis() + VMIX_RESPONSE_TIMEOUT;
  while ((apiClient.connected() || apiClient.available()) && millis() < deadline) {
    while (apiClient.available()) {
      response += static_cast<char>(apiClient.read());

      if (response.length() > 32768) {
        apiClient.stop();
        errorMessage = "vMix API response is too large";
        return false;
      }

      deadline = millis() + VMIX_RESPONSE_TIMEOUT;
    }

    delay(1);
  }

  apiClient.stop();

  int headerEnd = response.indexOf("\r\n\r\n");
  if (headerEnd < 0) {
    errorMessage = "Invalid vMix HTTP response";
    return false;
  }

  String headers = response.substring(0, headerEnd);
  if (headers.indexOf(" 200 ") < 0) {
    errorMessage = "The vMix API returned an HTTP error";
    return false;
  }

  xml = response.substring(headerEnd + 4);
  xml.trim();

  if (xml.indexOf("<vmix") < 0 || xml.indexOf("<inputs>") < 0) {
    errorMessage = "Invalid or incomplete vMix XML";
    return false;
  }

  return true;
}

bool fillVMixInputsFromXml(const String& xml, JsonArray inputs) {
  int searchIndex = 0;
  bool foundAny = false;

  while (true) {
    int tagStart = xml.indexOf("<input ", searchIndex);

    if (tagStart < 0) {
      break;
    }

    int tagEnd = xml.indexOf('>', tagStart);
    if (tagEnd < 0) {
      break;
    }

    String tag = xml.substring(tagStart, tagEnd + 1);
    String number = extractXmlAttribute(tag, "number");

    if (number.length() > 0) {
      JsonObject input = inputs.createNestedObject();
      input["number"] = number;
      input["title"] = extractXmlAttribute(tag, "title");
      input["key"] = extractXmlAttribute(tag, "key");
      input["type"] = extractXmlAttribute(tag, "type");
      foundAny = true;
    }

    searchIndex = tagEnd + 1;
  }

  return foundAny;
}

int findVMixInputNumberByKey(const String& xml, const char* targetKey, String& matchedTitle) {
  if (targetKey == nullptr || strlen(targetKey) == 0) {
    return 0;
  }

  int searchIndex = 0;

  while (true) {
    int tagStart = xml.indexOf("<input ", searchIndex);

    if (tagStart < 0) {
      break;
    }

    int tagEnd = xml.indexOf('>', tagStart);
    if (tagEnd < 0) {
      break;
    }

    String tag = xml.substring(tagStart, tagEnd + 1);
    String key = extractXmlAttribute(tag, "key");

    if (key.equals(String(targetKey))) {
      String number = extractXmlAttribute(tag, "number");
      matchedTitle = extractXmlAttribute(tag, "title");
      return number.toInt();
    }

    searchIndex = tagEnd + 1;
  }

  return 0;
}

void resetResolvedVMixInput() {
  lastVMixInputRefresh = 0;

  if (config.vmix_track_by_key) {
    resolvedVMixInputNumber = 0;
    resolvedVMixInputKey = String(config.vmix_input_key);
    resolvedVMixInputTitle = String(config.vmix_input_title);
  } else {
    resolvedVMixInputNumber = atoi(config.vmix_input);
    resolvedVMixInputKey = "";
    resolvedVMixInputTitle = "";
  }
}

int getActiveVMixInputNumber() {
  if (config.vmix_track_by_key) {
    return resolvedVMixInputNumber;
  }

  return atoi(config.vmix_input);
}

bool applyTallyFromData(const String& tallyData, int activeInputNumber) {
  if (activeInputNumber <= 0) {
    LOG_WARN("vMix input not configured");
    return false;
  }

  int inputIndex = activeInputNumber - 1;

  if (inputIndex < 0) {
    LOG_WARN("Invalid vMix input: %d", activeInputNumber);
    return false;
  }

  if (inputIndex < tallyData.length()) {
    char state = tallyData.charAt(inputIndex);
    bool live = (state == '1' || state == '3');
    bool preview = (state == '2' || state == '3');

    setTally(live, preview);
    return true;
  }

  LOG_WARN("Input %d out of range (max: %d)", activeInputNumber, tallyData.length());
  return false;
}

bool refreshResolvedVMixInput(bool forceRefresh) {
  if (!config.vmix_track_by_key) {
    resolvedVMixInputNumber = atoi(config.vmix_input);
    resolvedVMixInputKey = "";
    resolvedVMixInputTitle = "";
    return resolvedVMixInputNumber > 0;
  }

  if (strlen(config.vmix_input_key) == 0) {
    resolvedVMixInputNumber = 0;
    resolvedVMixInputKey = String(config.vmix_input_key);
    resolvedVMixInputTitle = String(config.vmix_input_title);
    LOG_WARN("vMix source not found for key: %s", config.vmix_input_key);
    return false;
  }

  unsigned long refreshIntervalMs = static_cast<unsigned long>(config.vmix_key_refresh_seconds) * 1000UL;
  if (!forceRefresh && lastVMixInputRefresh != 0 && millis() - lastVMixInputRefresh < refreshIntervalMs) {
    return resolvedVMixInputNumber > 0;
  }

  lastVMixInputRefresh = millis();

  String xml;
  String errorMessage;
  if (!fetchVMixApiXml(config.vmix_host, config.vmix_port, xml, errorMessage)) {
    LOG_WARN("Unable to resolve vMix key: %s", errorMessage.c_str());
    return resolvedVMixInputNumber > 0;
  }

  String matchedTitle;
  int matchedNumber = findVMixInputNumberByKey(xml, config.vmix_input_key, matchedTitle);

  if (matchedNumber <= 0) {
    resolvedVMixInputNumber = 0;
    resolvedVMixInputKey = String(config.vmix_input_key);
    resolvedVMixInputTitle = String(config.vmix_input_title);
    LOG_WARN("vMix source not found for key: %s", config.vmix_input_key);
    return false;
  }

  bool inputChanged = matchedNumber != resolvedVMixInputNumber;
  bool titleChanged = !matchedTitle.equals(resolvedVMixInputTitle);

  if (inputChanged || titleChanged) {
    LOG_VMIX("Tracked source resolved: %s -> input %d (%s)", config.vmix_input_key, matchedNumber, matchedTitle.c_str());
  }

  resolvedVMixInputNumber = matchedNumber;
  resolvedVMixInputKey = String(config.vmix_input_key);
  resolvedVMixInputTitle = matchedTitle.length() > 0 ? matchedTitle : String(config.vmix_input_title);

  if (inputChanged && lastVMixTallyData.length() > 0) {
    applyTallyFromData(lastVMixTallyData, matchedNumber);
  }

  return true;
}

void parseVMix(const String& cmd) {
  if (!cmd.startsWith("TALLY OK")) return;
  if (cmd.length() < 10) return;

  String tallyData = cmd.substring(9);
  tallyData.trim();
  lastVMixTallyData = tallyData;

  int activeInputNumber = getActiveVMixInputNumber();

  if (activeInputNumber <= 0 && config.vmix_track_by_key) {
    refreshResolvedVMixInput(false);
    activeInputNumber = getActiveVMixInputNumber();
  }

  applyTallyFromData(tallyData, activeInputNumber);
}
