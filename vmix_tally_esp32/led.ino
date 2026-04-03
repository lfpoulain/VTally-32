const uint8_t DIGIT_BITMAPS_8X8[][8] = {
  {0b00111100, 0b01100110, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01100110, 0b00111100},
  {0b00011000, 0b00111000, 0b01111000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b01111110},
  {0b00111100, 0b01100110, 0b00000110, 0b00001100, 0b00011000, 0b00110000, 0b01100000, 0b01111110},
  {0b00111100, 0b01100110, 0b00000110, 0b00011100, 0b00000110, 0b00000110, 0b01100110, 0b00111100},
  {0b00001100, 0b00011100, 0b00111100, 0b01101100, 0b11001100, 0b11111110, 0b00001100, 0b00001100},
  {0b01111110, 0b01100000, 0b01100000, 0b01111100, 0b00000110, 0b00000110, 0b01100110, 0b00111100},
  {0b00111100, 0b01100110, 0b01100000, 0b01111100, 0b01100110, 0b01100110, 0b01100110, 0b00111100},
  {0b01111110, 0b00000110, 0b00001100, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00110000},
  {0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b01100110, 0b01100110, 0b01100110, 0b00111100},
  {0b00111100, 0b01100110, 0b01100110, 0b00111110, 0b00000110, 0b00000110, 0b01100110, 0b00111100}
};

int mapMatrixIndex(int x, int y) {
  if (x < 0 || x >= 8 || y < 0 || y >= 8) {
    return -1;
  }

  return (y * 8) + x;
}

void fillAllPixels(uint32_t color) {
  for (int i = 0; i < config.led_count; i++) {
    strip->setPixelColor(i, color);
  }
}

void setMatrixPixel(int x, int y, uint32_t color) {
  int index = mapMatrixIndex(x, y);
  if (index >= 0 && index < config.led_count) {
    strip->setPixelColor(index, color);
  }
}

void fillMatrixRect(int x0, int y0, int x1, int y1, uint32_t color) {
  for (int y = y0; y <= y1; y++) {
    for (int x = x0; x <= x1; x++) {
      setMatrixPixel(x, y, color);
    }
  }
}

void drawDebugDigit(uint8_t digit, uint32_t color) {
  fillAllPixels(0);

  if (digit > 9) {
    digit = 9;
  }

  for (int y = 0; y < 8; y++) {
    uint8_t row = DIGIT_BITMAPS_8X8[digit][y];
    for (int x = 0; x < 8; x++) {
      if (row & (1 << (7 - x))) {
        setMatrixPixel(x, y, color);
      }
    }
  }
}

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

  if (config.display_mode == DISPLAY_MODE_MATRIX_8X8 && config.live_debug && !isLive && !isPreview) {
    if (debugStageCode == 3 || debugStageCode == 4 || debugStageCode == 5) {
      fillAllPixels(color);
      strip->show();
      return;
    }

    uint32_t debugColor = 0x3B82F6;
    if (debugStageCode == 4) {
      debugColor = config.preview_color;
    } else if (debugStageCode == 5) {
      debugColor = config.live_color;
    } else if (debugStageCode == 6 || debugStageCode == 9) {
      debugColor = 0xFFFF00;
    } else if (debugStageCode == 7) {
      debugColor = config.off_color;
    } else if (debugStageCode == 8) {
      debugColor = 0x22C55E;
    }
    drawDebugDigit(debugStageCode, debugColor);
    strip->show();
    return;
  }

  if (config.display_mode == DISPLAY_MODE_MATRIX_8X8) {
    fillAllPixels(color);
    strip->show();
    return;
  }

  if (strip->getPixelColor(0) != color) {
    for (int i = 0; i < config.led_count; i++) {
      strip->setPixelColor(i, color);
    }
    changed = true;
  }

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

  if (live) {
    debugStageCode = 5;
    debugStageLabel = "TALLY_LIVE";
  } else if (preview) {
    debugStageCode = 4;
    debugStageLabel = "TALLY_PREVIEW";
  } else if (vmixConnected) {
    debugStageCode = 3;
    debugStageLabel = "VMIX_CONNECTED";
  }

  applyTallyState();
}
