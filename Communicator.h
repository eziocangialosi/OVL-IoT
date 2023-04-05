/**
 * @file        Communicator.h
 * @brief       Definition of Communicator class
 * @details     This class manage everything related to communication (GPRS & MQTT)
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#define TRACKER_CALLBACK_SIG std::function<void(String)> calledWhenMsg

#include "SerialDebug.h"
#include "LedIndicator.h"

#include <PubSubClient.h>
#include <TinyGsmClient.h>

class Communicator
  {
    public:
      //Constructor, pointor to serial debug class needed to send debug info, pointor to led sign needed to debug via led codes
      Communicator(SerialDebug* apSerialDebug, LedIndicator* apLightSign);
      bool unlockSIM(); //(Re)unlock sim card
      bool connectGPRS(); //(Re)connect GPRS
      bool connectMQTT(); //(Re)connect MQTT
      bool sendMqtt(String aFrame); //Send an MQTT message on the TX topic
      bool getIsConnected(); //Get if mqtt + gprs are connected and if the hand check as success
      bool getWaitingForHandCheck(); //Get if we wait for the handcheck answer
      unsigned long getLastHandCheckRq(); //Get the timestamp of the last handcheck request
      bool getHandCheckSuccess(); //Get if the handcheck has success
      void execMqttLoop(); //Run the Mqtt loop (Mandatory to recived mqqt messages)
      void handCheckHandle(String msg); //Method who handle the handcheck answer
      void setCallWhenMsg(TRACKER_CALLBACK_SIG); //Method to set the method called when a mqtt message is recived
      unsigned int getBatteryPercent(); //Getter for the battery percentage (according to the GSM module)
      bool getBatteryInCharge(); // Getter for if the battery is(n't) in charge (according to the GSM module)
      void autoReconnect(); //Try to restore the connection (GPRS & MQTT)
      void tryHandCheck(); //Try to hand check with server
      bool getSimLocked();
      bool connectNetwork();
      bool sendDebugMqtt(String aPayload);
      
    private:
      SerialDebug* pUsbDebug;
      bool waitingForHandCheck = false; //If we wait for the Hand Check validation
      bool handCheckSuccess = false; //If the Hand Check as success
      bool wasInit = false; //If the obj was initialize
      bool isFirstHandCheck = true;
      unsigned long lastHandCheckRq = 0; //Timestamp of the last HandCheck RQ
      uint32_t lastReconnectAttempt = 0; //Timestamp of the last reconnect attempt
      TinyGsm* pModem; //Pontor to TinyGsm class (GSM/GPRS Module)
      TinyGsmClient* pClient; //Pointor to TinyGsmClient class (Interface between MQTT and GSM/GPRS module)
      PubSubClient*  pMqtt; //Pointor to PubSubClient class (MQTT Client)
      LedIndicator* pLightSign; //Pointor to led debug obj
      TRACKER_CALLBACK_SIG; //Callback function stored

      void mqttCallback(char* topic, byte* payload, unsigned int len); //Method called when a mqtt message is recvied
  };

#endif 
