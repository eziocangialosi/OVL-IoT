#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <PubSubClient.h>
#include <TinyGsmClient.h>

#include "Tracker.h"

class Communicator
  {
    public:
      Communicator(SerialDebug* apSerialDebug/*,const sParam asParam*/);
      bool connectNetwork();
      bool sendMqtt(String aFrame);
      static void Tracker::whenMqttRx(String payload);
      bool getIsConnected();
    private:
      SerialDebug* pUsbDebug;
      bool isConnected = false;
      //sParam* psParam;      
      
  };

#endif 
