void applyTallyState() {
  if (strip == nullptr) {
    return;
  }

  uint32_t color = config.off_color;
  if (isLive) {
    color = config.live_color;
  } else if (isPreview) {
    color = config.preview_color;
  }

  bool changed = false;

  if (config.brightness != lastBrightness) {
    strip->setBrightness(config.brightness);
    lastBrightness = config.brightness;
    changed = true;
  }

  // Vérifier si la couleur d'au moins un pixel a changé
  if (strip->getPixelColor(0) != color) {
    for (int i = 0; i < config.led_count; i++) {
      strip->setPixelColor(i, color);
    }
    changed = true;
  }

  // Ne mettre à jour le strip que si nécessaire
  if (changed) {
    strip->show();
  }
}

void setTally(bool live, bool preview) {
  if (live == isLive && preview == isPreview) return;

  LOG_DEBUG("Tally: %s → Live:%s Preview:%s",
    (isLive ? "LIVE" : (isPreview ? "PREVIEW" : "OFF")),
    (live ? "OUI" : "NON"),
    (preview ? "OUI" : "NON"));

  isLive = live;
  isPreview = preview;

  String state = "OFF";

  if (live && preview) {
    state = "LIVE+PREVIEW";
  } else if (live) {
    state = "LIVE";
  } else if (preview) {
    state = "PREVIEW";
  }

  if (state != lastState) {
    LOG_INFO("TALLY: %s → %s [Input %s]", lastState.c_str(), state.c_str(), config.vmix_input);
    lastState = state;
  }

  applyTallyState();
}
