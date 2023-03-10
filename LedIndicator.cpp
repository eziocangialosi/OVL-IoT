#include "LedIndicator.h"

LedIndicator::LedIndicator(uint8_t ledPin){
  FastLED.addLeds<NEOPIXEL, PIN>(&this->led, NUM_LEDS);
  this->setToBlack();
  this->led = CRGB::Orange;
  FastLED.show();
  delay(100);
}

void LedIndicator::setToBlack(){
  this->led = CRGB::Black;
  FastLED.show();
  delay(100);
}
