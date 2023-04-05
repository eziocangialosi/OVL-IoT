/**
 * @file        LedIndicator.h
 * @brief       Definition of LedIndicator class
 * @details     This class is used to manage the firebeetle debug led
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <FastLED.h>

#define NUM_LEDS 1
#define PIN 5

class LedIndicator
  {
    public:
      LedIndicator();
      void setToBlack();
      void blink(const CRGB aColor);
      void blinkInfty(const CRGB aColor);
      void stopBlinking();
      void ledLoop();
      void setTo(const CRGB aColor);
      bool isBlinking();
      void killLoop(); //Infinite loop called when a fatal error occured
    private:
      CRGB led;
      bool waitForBlack = false;
      bool blinkNonStop = false;
      CRGB blinkColor = CRGB::Black;
  };

#endif
