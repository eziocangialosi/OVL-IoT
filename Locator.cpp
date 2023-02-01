#include "Locator.h"

Locator::Locator(SerialDebug* aUsbDebug){
  this->pUsbDebug = aUsbDebug;
  
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
    crntLat = lastLat;
    crntLon = lastLon;
    return true;
  }
  return false;
}

bool Locator::getPrtPos(float* aPrtLon, float* aPrtLat){
  if(protection_enable){
    this->prtLon = aPrtLon;
    this->prtLat = aPrtLat;
    return true;
  }
  return false;
}

byte Locator::watchDog(){
  byte rtn_byte = 0;
  if(gps->location.isValid()){
    if((lastLat != gps->location.lat() || lastLon != gps->location.lng()) ||
    (millis() - this->lastPosTime < this->interval * 1000)){
      this->rqPos();
      if(protection_enable){
        if((this->lastLat != this->prtLat)||(this->lastLon != this->prtLon)){
          rtn_byte += 2;
        }
      }
      rtn_byte += 1;
    }
  }
  return rtn_byte;
}

void Locator::enterPrtMode(){
  this->interval = 60;
  this->rqPos();
  this->protection_enable = true;
  this->prtLon = this->lastLon;
  this->prtLat = this->lastLat;
}

void setIntervalAlmMode(unsigned int secs){
  this->interval = secs;
}
