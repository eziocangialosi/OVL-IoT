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
      this->lightSign->setTo(CRGB::Red);
      this->lightSign->killLoop();
    }
  }
}

void Tracker::actionInLoop(){
  this->cellular->execMqttLoop();
  this->lightSign->ledLoop();
  this->veh_charge_manager();
  
  this->batteryWatchDog();
  
  if(!this->cellular->getIsConnected() && !this->cellular->getWaitingForHandshake()){
    this->lightSign->setTo(CRGB::Purple);
    this->cellular->autoReconnect();
  }
  
  if(!this->cellular->getWaitingForHandshake() && this->cellular->getHandshakeSuccess()
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
    delay(250);
  }else if(this->cellular->getHandshakeSuccess() && !this->cellular->getWaitingForHandshake()
           && !this->positioning->getIsInit() && this->paramSetted){
    this->positioning->beg();
  }else if(!this->cellular->getHandshakeSuccess() && this->cellular->getWaitingForHandshake() && this->cellular->getLastHandshakeRq() + 5000 > millis()){
    delay(1);
  }else if(!this->paramSetted && !this->waitForParam && this->cellular->getHandshakeSuccess()){
    this->waitForParam = true;
    this->lightSign->setTo(CRGB::Orange);
    this->usbDebug->wrt("Requesting Param...");
    this->cellular->sendMqtt("STG-RQ");
  }else if(this->cellular->getWaitingForHandshake() && ((this->cellular->getLastHandshakeRq() + 5000 )< millis())
           && !this->cellular->getHandshakeSuccess()){
    this->lightSign->blink(CRGB::Orange);
    this->usbDebug->wrt("Retry to handshake...");
    this->cellular->tryHandshake();
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
  }else if(payload.startsWith("STG=")){
    this->parseParamFrame(payload);
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
  bool isError = false;
  if(payload.charAt(4) == '1'){
    this->allowChargeOnVehicle = true;
  }else if(payload.charAt(4) == '0'){
    this->allowChargeOnVehicle = false;
    digitalWrite(VEH_ALIM_RELAY_PIN, LOW);
  }else{
    isError = true;
  }
  
  if(payload.charAt(6) == '1' && !isError){
    this->eco_mode = true;
  }else if(payload.charAt(6) == '0' && !isError){
    this->eco_mode = false;
  }else{
    isError = true;
  }

  if((payload.charAt(8) == '1' || payload.charAt(8) == '0') && !isError){
    this->mqtt_whenPrt(payload.charAt(8));
  }else{
    isError = true;
  }

  if(!isError){
    String str_diam = payload.substring(10);
    unsigned int recivedDiam = str_diam.toInt();
    this->usbDebug->wrt_inline("Safe zone diameter set to : ");
    this->usbDebug->wrt_inline(String(recivedDiam));
    this->usbDebug->wrt("m");
    this->positioning->setSafeZoneDiam(recivedDiam);
    this->usbDebug->wrt("Param recived !");
    this->cellular->sendMqtt("STG-ACK");
    this->paramSetted = true;
    this->waitForParam = false;
  }else{
    this->cellular->sendMqtt("ERR");
  }
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

void Tracker::mqtt_whenPrt(char aValue){
  if(aValue == '1' && !this->positioning->isProtectionEnable() && this->positioning->getIsInit()){
    this->positioning->enterPrtMode();
    this->usbDebug->wrt("Server enable protection mode");
    if(this->positioning->gpsIsFixed()){
      this->cellular->sendMqtt("PRT-ACK");
    }else{
      this->usbDebug->wrt("Caution, protection mode is limited, currently no gps fix...");
      this->cellular->sendMqtt("PRT-LIM");
      this->cellular->sendDebugMqtt("Caution protection is limited");
    }
  }else if(aValue == '0' && this->positioning->isProtectionEnable() && this->positioning->getIsInit()){
    this->positioning->quitPrtMode();
    this->usbDebug->wrt("Server disable protection mode");
    this->alarm_is_on = false;
    this->cellular->sendMqtt("PRT-ACK");
  }else if(aValue != '0' && !this->paramSetted && this->positioning->getIsInit()){
    this->cellular->sendMqtt("PRT-ERR");
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

void Tracker::batteryWatchDog(){
  unsigned int batVolt = this->cellular->getBatteryVolt();
  if(batVolt < LOW_VOLTAGE && batVolt > 1000){
    this->cellular->sendMqtt("BAT-LOW");
    this->cellular->sendDebugMqtt("Battery too low, powering down");
    this->usbDebug->wrt("Battery too low, powering down");
    this->lightSign->setTo(CRGB::Red);
    this->lightSign->killLoop();
  }
}
