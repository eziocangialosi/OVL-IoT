#include "Communicator.h"

Communicator::Communicator(SerialDebug* apSerialDebug){
  this->pUsbDebug = apSerialDebug;
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

  this->pUsbDebug->wrt("Unlocking sim...");
  if (GSM_PIN && this->pModem->getSimStatus() != 3) {
    if(this->pModem->simUnlock(GSM_PIN)){
      this->pUsbDebug->wrt("Sim unlocked");
    }else{
      this->pUsbDebug->wrt("Fatal err: Unlocking failed");
      return;
    }
  }
  this->pUsbDebug->wrt_inline("Waiting for network...");
  if (!this->pModem->waitForNetwork()) {
        this->pUsbDebug->wrt(" fail");
        delay(10000);
        return;
  }
  this->pUsbDebug->wrt(" success");
  if (this->pModem->isNetworkConnected()) { this->pUsbDebug->wrt("Network connected"); }
}

bool Communicator::connectGPRS(){
  this->pUsbDebug->wrt_inline("Connecting to ");
  this->pUsbDebug->wrt_inline(String(APN));
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
    
    this->pUsbDebug->wrt_inline("Connecting to broker : ");
    this->pUsbDebug->wrt_inline(BROKER);
    this->pUsbDebug->wrt_inline(" ");
    
    if(this->pMqtt->connect("IoT")){
      this->pUsbDebug->wrt("success !");
      this->pUsbDebug->wrt("Try HandCheck...");
      this->waitingForHandCheck = true;
      this->pMqtt->subscribe(TOPIC_RX);
      this->lastHandCheckRq = millis();
      this->pMqtt->publish(TOPIC_TX, "SYN");
      this->pMqtt->loop();
      return true;
    }else{
      this->pUsbDebug->wrt("failed");
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
