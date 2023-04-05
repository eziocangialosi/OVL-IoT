/**
 * @file        Tracker.cpp
 * @brief       All methods code for Tracker class
 * @details     Methods code for all tracker algorithm
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#include "Tracker.h"

void Tracker::beg(){
  pinMode(VEH_ALIM_SENSOR_PIN, INPUT);
  pinMode(VEH_ALIM_RELAY_PIN, OUTPUT);
  this->usbDebug = new SerialDebug(USB_BAUD);
  this->lightSign = new LedIndicator();
  this->cellular = new Communicator(usbDebug, lightSign);
  using std::placeholders::_1;
  this->cellular->setCallWhenMsg(std::bind(&Tracker::whenMqttRx,this,_1));
  this->positioning = new Locator(usbDebug, lightSign);
  if(this->cellular->connectGPRS()){
    if(!this->cellular->connectMQTT()){
      //return;
    }
  }else{
    //return;
  }
}

void Tracker::actionInLoop(){
  this->cellular->execMqttLoop();
  this->lightSign->ledLoop();
  this->veh_charge_manager();

  if(!this->cellular->getIsConnected() && !this->cellular->getWaitingForHandCheck()){
    this->lightSign->setTo(CRGB::Purple);
    this->cellular->autoReconnect();
  }
  
  if(!this->cellular->getWaitingForHandCheck() && this->cellular->getHandCheckSuccess()
  && this->positioning->getIsInit() && this->paramSetted){
    byte pos_value = this->positioning->watchDog();
    if(pos_value & 0x02){
      this->beginAlarm();
    }
    if((pos_value & 0x01) && !this->pos_as_been_rq){
      float getLat = 0;
      float getLon = 0;
      this->usbDebug->wrt("Position changed");
      this->positioning->getPos(&getLat, &getLon);
      this->sendPos(getLat, getLon);
    }
    
  }else if(this->cellular->getHandCheckSuccess() && !this->cellular->getWaitingForHandCheck()
           && !this->positioning->getIsInit() && this->paramSetted){
    this->positioning->beg();
  }else if(!this->cellular->getHandCheckSuccess() && this->cellular->getWaitingForHandCheck() && this->cellular->getLastHandCheckRq() + 5000 > millis()){
    delay(1);
  }else if(!this->paramSetted && !this->waitForParam && this->cellular->getHandCheckSuccess()){
    this->waitForParam = true;
    this->lightSign->setTo(CRGB::Orange);
    this->usbDebug->wrt("Requesting Param...");
    this->cellular->sendMqtt("STG-RQ");
  }else if(this->cellular->getWaitingForHandCheck() && this->cellular->getLastHandCheckRq() + 5000 < millis()
           && !this->cellular->getHandCheckSuccess()){
    this->lightSign->blink(CRGB::Orange);
    this->usbDebug->wrt("Retry to HandCheck...");
    this->cellular->tryHandCheck();
  }

  this->pos_as_been_rq = false;
}

void Tracker::beginAlarm(){
  if(!alarm_is_on){
    this->alarm_is_on = true;
    this->positioning->setInterval(DEFAULT_INTERVAL_WHEN_ALM);
    this->positioning->setMinimalInterval(DEFAULT_INTERVAL_WHEN_ALM);
    this->usbDebug->wrt("Alert ! Tracker outside safe zone !");
    this->cellular->sendMqtt("ALM");
  }
}

void Tracker::whenMqttRx(String payload){
  this->lightSign->blink(CRGB::Green);
  if(payload == "PING"){
    this->usbDebug->wrt("Recived ping"); 
    this->cellular->sendMqtt("PONG");
  }else if(payload == "POS-RQ"){
    this->mqtt_whenPosRq();
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
    this->mqtt_whenPrt(payload.charAt(4));
  }else if(payload == "SFZ-RQ"){
    this->mqtt_whenSfzRq();
  }else if(payload == "ALM-ACK"){
    this->usbDebug->wrt("Server acknoweldge recived alarm");
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
  if(payload.charAt(4) == '1'){
    this->allowChargeOnVehicle = true;
  }else{
    this->allowChargeOnVehicle = false;
    digitalWrite(VEH_ALIM_RELAY_PIN, LOW);
  }
  
  if(payload.charAt(6) == '1'){
    this->eco_mode = true;
  }else{
    this->eco_mode = false;
  }
  
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

void Tracker::veh_charge_manager(){
  if(allowChargeOnVehicle){
    if(old_veh_alim_state != digitalRead(VEH_ALIM_SENSOR_PIN)){
      old_veh_alim_state = digitalRead(VEH_ALIM_SENSOR_PIN);
      if(old_veh_alim_state == HIGH){
        this->usbDebug->wrt("Detection of power from vehicle");
        veh_alim_time = millis();
      }else{
        digitalWrite(VEH_ALIM_RELAY_PIN, LOW);
      }
    }if((veh_alim_time + (DELAY_CHG_VEH*1000)) < millis()){
      if(old_veh_alim_state == HIGH && !isOnCharge){
        this->usbDebug->wrt("Begin charge on vehicle battery");
        digitalWrite(VEH_ALIM_RELAY_PIN, HIGH);
      }else if(old_veh_alim_state == LOW && isOnCharge){
        this->usbDebug->wrt("Lost power from vehicle battery");
        digitalWrite(VEH_ALIM_RELAY_PIN, LOW);
      }
    }
  }
}

void Tracker::mqtt_whenPosRq(){
  float posLon = 0;
  float posLat = 0;
  if(this->positioning->getPos(&posLat, &posLon)){
    this->usbDebug->wrt("Server Request POS");
    this->sendPos(posLat, posLon);
    this->pos_as_been_rq = true;
  }else{
    this->usbDebug->wrt("Position cannot be send...");
    this->cellular->sendMqtt("POS-ERR");
  }
}

void Tracker::mqtt_whenPrt(char value){
  if(value == '1' && !this->positioning->isProtectionEnable()){
    if(this->positioning->gpsIsFixed()){
      this->positioning->enterPrtMode();
      this->usbDebug->wrt("Server enable protection mode");
      this->cellular->sendMqtt("PRT-ACK");
    }else{
      this->usbDebug->wrt("Cannot enable protection mode");
      this->cellular->sendMqtt("PRT-ERR");
    }
  }else if(value == '0' && this->positioning->isProtectionEnable()){
    this->positioning->quitPrtMode();
    this->usbDebug->wrt("Server disable protection mode");
    this->alarm_is_on = false;
    this->cellular->sendMqtt("PRT-ACK");
  }else{
    this->cellular->sendMqtt("ERR");
    this->usbDebug->wrt("Cannot edit protection mode");
  }
}

void Tracker::mqtt_whenSfzRq(){
  this->usbDebug->wrt("Server Request for safe zone position");
  float sfzLat;
  float sfzLon;
  String msg;
  if(this->positioning->getPrtPos(&sfzLat, &sfzLon)){
    msg = "SFZ=";
    msg += String(sfzLat,10);
    msg += ",";
    msg += String(sfzLon,10);
    this->usbDebug->wrt("SafeZone position sended");
  }else{
    this->usbDebug->wrt("Cannot send safezone position");
    msg = "ERR";
  }
  this->cellular->sendMqtt(msg);
}
