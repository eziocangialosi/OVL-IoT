#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <FastLED.h>

#define NUM_LEDS 1
#define PIN 5

class LedIndicator
  {
    public:
      LedIndicator(uint8_t ledPin);
      void setToBlack();
      void blink(const CRGB aColor);
      void blinkInfty(const CRGB aColor);
      void stopBlinking();
      void ledLoop();
      void setTo(const CRGB aColor);
      bool isBlinking();
    private:
      CRGB led;
      bool waitForBlack = false;
      bool blinkNonStop = false;
      CRGB blinkColor = CRGB::Black;
  };

#endif
