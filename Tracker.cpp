#include "Tracker.h"

void Tracker::beg(){
  this->usbDebug = new SerialDebug(115200);
  this->positioning = new Locator(usbDebug);
}

bool Tracker::actionInLoop(){
  byte pos_value = this->positioning->watchDog();
  if(pos_value & 0x02){
    this->beginAlarm();
  }
  if(pos_value & 0x01){
    float getLat = 0;
    float getLon = 0;
    this->usbDebug->wrt("New current pos :");
    this->positioning->getPos(&getLat, &getLon);
    this->usbDebug->wrt_inline(String(getLat,10));
    this->usbDebug->wrt_inline(",");
    this->usbDebug->wrt(String(getLon,10));
    this->usbDebug->wrt("");
  }
  return true;  
}

void Tracker::beginAlarm(){
  if(!alarm_is_on){
    this->alarm_is_on = true;
    this->positioning->setInterval(5);
    this->usbDebug->wrt("Alert ! Tracker outside safe zone !");
    this->cellular->sendMqtt("ALM");
  }
}

void Tracker::whenMqttRx(String payload){
  
}
