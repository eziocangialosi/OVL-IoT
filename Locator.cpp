#include "Locator.h"

Locator::Locator(SerialDebug* aUsbDebug){
  this->pUsbDebug = aUsbDebug;
  Serial2.begin(9600);
  
  pUsbDebug->wrt("GPS initializing");
  pUsbDebug->wrt_inline("Lib ver:");
  pUsbDebug->wrt(TinyGPSPlus::libraryVersion());
  
  this->gps = new TinyGPSPlus();

  pUsbDebug->wrt("Wait for GPS fix");
  
  this->acquire_nmea_while_ms(1000);
  if(this->waitGPSFix(60)){
    pUsbDebug->wrt("GPS OK");
    this->lastPosTime = millis();
    this->interval = 60 * 5; //5min
  }else{
    pUsbDebug->wrt("GPS FAILED");
  }
  
}

// This custom version of delay() ensures that the gps object
// is being "fed".
void Locator::acquire_nmea_while_ms(unsigned long ms){
  unsigned long start = millis();
  do 
  {
    while (Serial2.available())
      gps->encode(Serial2.read());
  } while (millis() - start < ms);
}

bool Locator::waitGPSFix(unsigned long max_wait_sec){
  unsigned long start = millis();
  do{
    this->acquire_nmea_while_ms(1000);
  }while((millis() - start < max_wait_sec * 1000) && !gps->location.isValid());

  return gps->location.isValid();
}

void Locator::rqPos(){
  this->lastLat = gps->location.lat();
  this->lastLon = gps->location.lng();
}

bool Locator::getPos(float* crntLon, float* crntLat){
  if(gps->location.isValid()){
    this->lastPosTime = millis();
    *crntLat = lastLat;
    *crntLon = lastLon;
    return true;
  }
  return false;
}

bool Locator::getPrtPos(float* aPrtLon, float* aPrtLat){
  if(protection_enable){
    this->prtLon = *aPrtLon;
    this->prtLat = *aPrtLat;
    return true;
  }
  return false;
}

byte Locator::watchDog(){
  byte rtn_byte = 0;
  acquire_nmea_while_ms(1000);
  if(gps->location.isValid()){
    //if the position has changed or date more than 5 min
    if((this->round_float_dp(lastLat,DP_POS) != this->round_float_dp(gps->location.lat(),DP_POS) ||
    this->round_float_dp(lastLon,DP_POS) != this->round_float_dp(gps->location.lng(),DP_POS)) ||
    (millis() - this->lastPosTime > this->interval * 1000))
    {
      this->rqPos();
      if(protection_enable){
        if(TinyGPSPlus::distanceBetween(lastLat,lastLon,prtLat,prtLon) > this->safeZoneDiam){
          rtn_byte += 2;
          this->pUsbDebug->wrt_inline("Distance with safeZone center : ");
          this->pUsbDebug->wrt_inline(String(TinyGPSPlus::distanceBetween(lastLat,lastLon,prtLat,prtLon)));
          this->pUsbDebug->wrt("m");
        }
      }
      rtn_byte += 1;
    }

  }
  return rtn_byte;
}

void Locator::enterPrtMode(){
  this->rqPos();
  this->protection_enable = true;
  this->prtLon = this->lastLon;
  this->prtLat = this->lastLat;
}

void Locator::setInterval(unsigned int secs){
  this->interval = secs;
}

float Locator::round_float_dp(float in_value, int decimal_place){
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}