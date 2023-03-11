#include "LedIndicator.h"

LedIndicator::LedIndicator(uint8_t ledPin){
  FastLED.addLeds<NEOPIXEL, PIN>(&this->led, NUM_LEDS);
  this->setToBlack();
  this->setTo(CRGB::Orange);
}

void LedIndicator::ledLoop(){
  if(this->waitForBlack){
    this->setToBlack();
    this->waitForBlack = false;
    if(this->blinkNonStop){
      this->lastBlink = millis();
    }
  }else if(this->blinkNonStop){
    this->setTo(this->blinkColor);
    this->waitForBlack = true;
  }
}

void LedIndicator::setToBlack(){
  this->setTo(CRGB::Black);
}

void LedIndicator::setTo(const CRGB aColor){
  this->led = aColor;
  FastLED.show();
  delay(1);
}

void LedIndicator::blink(const CRGB aColor){
  this->setTo(aColor);
  this->waitForBlack = true;
  this->blinkNonStop = false;
}
void LedIndicator::blinkInfty(const CRGB aColor){
  this->setTo(aColor);
  this->blinkColor = aColor;
  this->waitForBlack = true;
  this->blinkNonStop = true;
}
void LedIndicator::stopBlinking(){
  this->waitForBlack = false;
  this->blinkNonStop = false;
  this->blinkColor = CRGB::Black;
  this->setToBlack();
}

bool LedIndicator::isBlinking(){
  return this->blinkNonStop;
}
