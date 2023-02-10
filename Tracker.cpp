#include "Tracker.h"

void Tracker::beg(){
  this->usbDebug = new SerialDebug(USB_BAUD);
  this->cellular = new Communicator(usbDebug);
  using std::placeholders::_1;
  this->cellular->setCallWhenMsg(std::bind(&Tracker::whenMqttRx,this,_1));
  this->positioning = new Locator(usbDebug);
  if(this->cellular->connectGPRS()){
    if(!this->cellular->connectMQTT()){
      return;
    }
  }else{
    return;
  }
}

void Tracker::actionInLoop(){
  this->cellular->execMqttLoop();

  if(!this->cellular->getIsConnected() && !this->cellular->getWaitingForHandCheck()){
    this->usbDebug->wrt("Connection with broker lost, trying to reconnect...");
    this->cellular->connectMQTT();
  }
  
  if(!this->cellular->getWaitingForHandCheck() && this->cellular->getLastHandCheckRq() + 5000 < millis()
  && this->cellular->getHandCheckSuccess() && this->positioning->getIsInit() && this->paramSetted){
    byte pos_value = this->positioning->watchDog();
    if(pos_value & 0x02){
      this->beginAlarm();
    }
    if((pos_value & 0x01) && !this->prt_as_been_rq){
      float getLat = 0;
      float getLon = 0;
      this->usbDebug->wrt("Position changed");
      this->positioning->getPos(&getLat, &getLon);
      this->sendPos(getLat, getLon);
    } 
  }else if(this->cellular->getHandCheckSuccess() && !this->cellular->getWaitingForHandCheck() && !this->positioning->getIsInit()){
    this->positioning->beg();
  }else if(!this->cellular->getHandCheckSuccess() && this->cellular->getWaitingForHandCheck() && this->cellular->getLastHandCheckRq() + 5000 > millis()){
    delay(1);
  }else if(!this->paramSetted && !this->waitForParam){
    this->waitForParam = true;
    this->usbDebug->wrt("Requesting Param...");
    this->cellular->sendMqtt("STG-RQ");
  }

  this->prt_as_been_rq = false;
}

void Tracker::beginAlarm(){
  if(!alarm_is_on){
    this->alarm_is_on = true;
    this->positioning->setInterval(DEFAULT_INTERVAL_WHEN_ALM);
    this->usbDebug->wrt("Alert ! Tracker outside safe zone !");
    this->cellular->sendMqtt("ALM");
  }
}

void Tracker::whenMqttRx(String payload){
  if(payload == "PING"){
    this->usbDebug->wrt("Recived ping"); 
    this->cellular->sendMqtt("PONG");
  }else if(payload == "POS-RQ"){
    float posLon = 0;
    float posLat = 0;
    if(this->positioning->getPos(&posLat, &posLon)){
      this->usbDebug->wrt("Server Request POS");
      this->sendPos(posLat, posLon);
      this->prt_as_been_rq = true;
    }else{
      this->usbDebug->wrt("Position cannot be send...");
      this->cellular->sendMqtt("POS-ERR");
    }
  }else if(payload.startsWith("STG=") && this->waitForParam){
    this->parseParamFrame(payload);
    this->paramSetted = true;
    this->waitForParam = false;
  }else if(payload == "POS-ACK"){
    this->usbDebug->wrt("Server acknoweldge recived position");
  }else if(payload == "STS-RQ"){
    this->usbDebug->wrt("Server Request for status");
    this->sendSts();    
  }else if(payload == "STS-ACK"){
    this->usbDebug->wrt("Server acknoweldge recived status");
  }else if(payload.startsWith("PRT=")){
    if(payload.charAt(4) == '1' && !this->positioning->isProtectionEnable()){
      this->positioning->enterPrtMode();
      this->usbDebug->wrt("Server enable protection mode");
    }else if(payload.charAt(4) == '0' && this->positioning->isProtectionEnable()){
      this->positioning->quitPrtMode();
      this->usbDebug->wrt("Server disable protection mode");
      alarm_is_on = false;
    }else{
      this->cellular->sendMqtt("ERR");
      this->usbDebug->wrt("Cannot edit protection mode");
    }
  }else{
    this->cellular->sendMqtt("ERR");
    this->usbDebug->wrt("Message interpretation failed");
  }
}

void Tracker::sendPos(float aLat, float aLon){
  String tmp = "POS=";
  tmp += String(aLat,10);
  tmp += ",";
  tmp += String(aLon,10);
  if(this->cellular->sendMqtt(tmp)){
    this->usbDebug->wrt("Position sended !");
  }else{
    this->usbDebug->wrt("Failed to send position");
  }
}

void Tracker::parseParamFrame(String payload){
  this->allowChargeOnVehicle = (payload.charAt(4) == '1');
  this->eco_mode = (payload.charAt(6) == '1');
  if(payload.charAt(8) == '1' && !this->positioning->isProtectionEnable()){
    this->positioning->enterPrtMode();
  }else if(payload.charAt(8) == '0' && this->positioning->isProtectionEnable()){
    this->positioning->quitPrtMode();
    alarm_is_on = false;
  }
  String str_diam = payload.substring(10);
  unsigned int recivedDiam = str_diam.toInt();
  this->usbDebug->wrt_inline("Safe zone diameter set to : ");
  this->usbDebug->wrt_inline(String(recivedDiam));
  this->usbDebug->wrt("m");
  this->positioning->setSafeZoneDiam(recivedDiam);
  this->usbDebug->wrt("Param recived !");
}

void Tracker::sendSts(){
  this->usbDebug->wrt("Sending status to srv");
  String rq = "STS=";
  rq += String(this->cellular->getBatteryPercent());
  rq += ",";
  rq += String(this->cellular->getBatteryInCharge());
  rq += ",";
  rq += String(this->allowChargeOnVehicle);
  rq += ",";
  rq += String(this->eco_mode);
  rq += ",";
  rq += String(this->positioning->isProtectionEnable());
  rq += ",";
  rq += String(this->alarm_is_on);
  rq += ",";
  rq += String(this->positioning->gpsIsFixed());
  if(!this->cellular->sendMqtt(rq)){
    this->usbDebug->wrt("Error when trying to send status to srv");
  }
}
