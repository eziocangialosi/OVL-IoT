#include "Locator.h"

Locator::Locator(SerialDebug* aUsbDebug, LedIndicator* apLightSign){
  this->pUsbDebug = aUsbDebug;
  this->pLightSign = apLightSign;
}

void Locator::beg(){
  Serial2.begin(GPS_BAUD);
  
  pUsbDebug->wrt("GPS initializing");
  pUsbDebug->wrt_inline("Lib ver:");
  pUsbDebug->wrt(TinyGPSPlus::libraryVersion());
  
  this->gps = new TinyGPSPlus();

  pUsbDebug->wrt("Wait for GPS fix");
  this->pLightSign->setTo(CRGB::Blue);
  this->acquire_nmea_while_ms(1000);
  if(this->waitGPSFix(60)){
    pUsbDebug->wrt("GPS OK");
    this->lastPosTime = millis();
    this->interval = DEFAULT_INTERVAL; //5min
    this->min_interval = MINIMAL_INTERVAL;
  }else{
    this->pUsbDebug->wrt("GPS FAILED");
    this->pUsbDebug->wrt_inline("Only have ");
    this->pUsbDebug->wrt_inline(String(this->gps->satellites.isValid()));
    this->pUsbDebug->wrt(" satellite(s)...");
    this->pLightSign->blinkInfty(CRGB::Blue);
  }
  this->isInit = true;
}

// This custom version of delay() ensures that the gps object
// is being "fed".
void Locator::acquire_nmea_while_ms(unsigned long ms){
  unsigned long start = millis();
  do 
  {
    while (Serial2.available())
      this->gps->encode(Serial2.read());
  } while (millis() - start < ms);
}

bool Locator::waitGPSFix(unsigned long max_wait_sec){
  unsigned long start = millis();
  do{
    this->acquire_nmea_while_ms(1000);
  }while((millis() - start < max_wait_sec * 1000) && !this->gps->location.isValid());

  return this->gps->location.isValid();
}

void Locator::rqPos(){
  this->lastLat = this->gps->location.lat();
  this->lastLon = this->gps->location.lng();
}

bool Locator::getPos(float* crntLat, float* crntLon){
  if(this->gps->location.isValid() && this->gps->location.age() < 2000){
    this->lastPosTime = millis();
    *crntLat = lastLat;
    *crntLon = lastLon;
    return true;
  }
  return false;
}

bool Locator::getPrtPos(float* aPrtLat, float* aPrtLon){
  if(protection_enable){
    *aPrtLat = this->prtLat;
    *aPrtLon = this->prtLon;
    return true;
  }
  return false;
}

byte Locator::watchDog(){
  byte rtn_byte = 0;
  acquire_nmea_while_ms(1000);
  if(this->gps->location.isValid() && this->gps->location.age() < 2000){
    if(this->pLightSign->isBlinking()){
      this->pLightSign->stopBlinking();
    }
    //if the position has changed or date more than 5 min
    if(((TinyGPSPlus::distanceBetween(lastLat,lastLon,this->gps->location.lat(),this->gps->location.lng()) >= DISTANCE_TRIG)
        && (millis() - this->lastPosTime > this->min_interval * 1000)) ||
        (millis() - this->lastPosTime > this->interval * 1000))
    {
      this->rqPos();
      if(protection_enable){
        if(TinyGPSPlus::distanceBetween(lastLat,lastLon,prtLat,prtLon) > (this->safeZoneDiam / 2)){
          rtn_byte += 2;
          this->pUsbDebug->wrt_inline("Distance with safeZone center : ");
          this->pUsbDebug->wrt_inline(String(TinyGPSPlus::distanceBetween(lastLat,lastLon,prtLat,prtLon)));
          this->pUsbDebug->wrt("m");
        }
      }
      rtn_byte += 1;
    }

  }else if(!this->pLightSign->isBlinking()){
    this->pLightSign->blinkInfty(CRGB::Blue);
  }
  return rtn_byte;
}

void Locator::enterPrtMode(){
  if(this->isInit){
    this->rqPos();
    this->protection_enable = true;
    this->prtLon = this->lastLon;
    this->prtLat = this->lastLat;
  } 
}

void Locator::setInterval(unsigned int secs){
  this->interval = secs;
}

void Locator::setMinimalInterval(unsigned int secs){
  this->min_interval = secs;
}

float Locator::round_float_dp(float in_value, int decimal_place){
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

bool Locator::getIsInit(){
  return this->isInit;
}

void Locator::setSafeZoneDiam(unsigned int aDiam){
  this->safeZoneDiam = aDiam;
}

bool Locator::isProtectionEnable(){
  return this->protection_enable;
}

void Locator::quitPrtMode(){
  this->rqPos();
  this->protection_enable = false;
  this->prtLon = 0;
  this->prtLat = 0;
  this->interval = DEFAULT_INTERVAL;
  this->min_interval = MINIMAL_INTERVAL;
}

bool Locator::gpsIsFixed(){
  return (this->gps->location.isValid() && this->gps->location.age() < 2000);
}
