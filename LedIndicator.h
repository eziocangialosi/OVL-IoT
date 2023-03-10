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
    private:
      CRGB led;
  };

#endif
