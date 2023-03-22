#include "Communicator.h"

Communicator::Communicator(SerialDebug* apSerialDebug, LedIndicator* apLightSign){
  this->pUsbDebug = apSerialDebug;
  this->pLightSign = apLightSign;
  this->pUsbDebug->wrt("GPRS initializing...");
  this->pModem = new TinyGsm(Serial1);
  this->pClient = new TinyGsmClient(*pModem);
  this->pMqtt = new PubSubClient(*pClient);
  
  Serial1.begin(GSM_BAUD);
  delay(6000);
  
  this->pModem->restart();

  String modemName = this->pModem->getModemName();
  this->pUsbDebug->wrt("Modem Name: ");
  this->pUsbDebug->wrt(modemName);

  if (this->getSimLocked()) {
    this->pUsbDebug->wrt_inline("Unlocking sim...");
    this->unlockSIM();
  }
  this->connectNetwork();
}

bool Communicator::unlockSIM(){
  if(this->pModem->simUnlock(GSM_PIN)){
      this->pUsbDebug->wrt(" success");
      return true;
    }else{
      this->pUsbDebug->wrt("");
      this->pUsbDebug->wrt("Fatal err: Unlocking failed");
      this->pLightSign->setTo(CRGB::Red);
      this->pLightSign->killLoop();
    }
    return false;
}

bool Communicator::connectGPRS(){
  this->pUsbDebug->wrt_inline("Connecting to ");
  this->pUsbDebug->wrt_inline(String(APN));
  this->pUsbDebug->wrt_inline("...");
  if (!this->pModem->gprsConnect(APN, GPRS_USER, GPRS_PSWD)) {
    this->pUsbDebug->wrt(" fail");
    delay(10000);
    return false;
  }
  this->pUsbDebug->wrt(" success");

  if (this->pModem->isGprsConnected()) {
    this->pUsbDebug->wrt("GPRS connected");
    return true;
  }else{
    return false;
  }
}

bool Communicator::connectMQTT(){
  if(this->pModem->isGprsConnected()){
    this->handCheckSuccess = false;
    if(!this->wasInit){
      this->pMqtt->setServer(BROKER, MQTT_PORT);
      using std::placeholders::_1;
      using std::placeholders::_2;
      using std::placeholders::_3;
      this->pMqtt->setCallback(std::bind(&Communicator::mqttCallback,this,_1,_2,_3));
      this->pMqtt->setKeepAlive(15);
      this->wasInit = true;
    }
    
    this->pUsbDebug->wrt_inline("Connecting to broker ");
    this->pUsbDebug->wrt_inline(BROKER);
    this->pUsbDebug->wrt_inline("...");
    
    if(this->pMqtt->connect(MQTT_USER, MQTT_USER, MQTT_PSWD)){
      this->pUsbDebug->wrt(" success");
      this->pUsbDebug->wrt("Try HandCheck...");
      this->waitingForHandCheck = true;
      this->pMqtt->subscribe(TOPIC_RX);
      this->tryHandCheck();
      this->pMqtt->loop();
      return true;
    }else{
      this->pUsbDebug->wrt(" fail");
      return false;
    }
  }
  return false;
}

void Communicator::mqttCallback(char* topic, byte* payload, unsigned int len) {
  String msg = "";
  for(unsigned int i = 0; i < len; i++){
    msg += (char)payload[i];
  }
  if(this->waitingForHandCheck){
    this->handCheckHandle(msg);
  }else{
    this->calledWhenMsg(msg);
  }
}

bool Communicator::getWaitingForHandCheck(){
  return this->waitingForHandCheck;
}

unsigned long Communicator::getLastHandCheckRq(){
  return this->lastHandCheckRq;
}

void Communicator::execMqttLoop(){
  this->pMqtt->loop();
}

bool Communicator::getIsConnected(){
  if(this->pMqtt->connected() && this->pModem->isGprsConnected()){
    return this->handCheckSuccess;
  }else{
    return false;
  }
}

void Communicator::handCheckHandle(String msg){
  if(msg == "SYN-ACK"){
    this->pUsbDebug->wrt("Hand Check OK");
    this->waitingForHandCheck = false;
    this->handCheckSuccess = true;
    this->sendMqtt("ACK");
  }
}

bool Communicator::getHandCheckSuccess(){
  return this->handCheckSuccess;
}

bool Communicator::sendMqtt(String aFrame){
  if(this->getIsConnected()){
    char buffer[aFrame.length() +1];
    aFrame.toCharArray(buffer, aFrame.length() +1);
    return this->pMqtt->publish(TOPIC_TX, buffer);
  }
  return false;
}

void Communicator::setCallWhenMsg(TRACKER_CALLBACK_SIG){
  this->calledWhenMsg = calledWhenMsg;
}

unsigned int Communicator::getBatteryPercent(){
  return this->pModem->getBattPercent();
}
bool Communicator::getBatteryInCharge(){
  return this->pModem->getBattChargeState();
}

void Communicator::autoReconnect(){
  /*if (this->getSimLocked()) {
    this->pUsbDebug->wrt_inline("Sim is locked, trying to unlock ");
    this->unlockSIM();
  }
  this->connectNetwork();*/
  
  if(!this->pModem->isGprsConnected()){
    this->pUsbDebug->wrt("Celluar connection lost, trying to reconnect...");
    this->connectGPRS();
  }
  if(!this->pMqtt->connected()){
    this->pUsbDebug->wrt("Connection with broker lost, trying to reconnect...");
    this->connectMQTT();
  }
}

void Communicator::tryHandCheck(){
  this->lastHandCheckRq = millis();
  this->pMqtt->publish(TOPIC_TX, "SYN");
}

bool Communicator::getSimLocked(){
  return ((GSM_PIN) && (this->pModem->getSimStatus() != 3));
}

bool Communicator::connectNetwork(){
  this->pUsbDebug->wrt_inline("Waiting for network...");
  if (!this->pModem->waitForNetwork()) {
    this->pUsbDebug->wrt(" fail");
    delay(10000);
    return false;
  }
  this->pUsbDebug->wrt(" success");
  if (this->pModem->isNetworkConnected()) {
    this->pUsbDebug->wrt("Network connected");
    return true;
  }
  return false;
}
