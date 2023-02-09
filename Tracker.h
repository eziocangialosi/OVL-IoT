#ifndef TRACKER_H
#define TRACKER_H

#include "SerialDebug.h"
#include "Locator.h"
#include "Communicator.h"

class Tracker
  {
    public:
      void beg();
      void actionInLoop();
    private:
      bool checkInitParam();
      void beginAlarm();
      void whenMqttRx(String payload);
      void sendPos(float aLon, float aLat);
      void parseParamFrame(String payload);

      bool alarm_is_on = false;
      bool waitForParam = false;
      bool paramSetted = false;
      bool allowChargeOnVehicle = false;
      bool eco_mode = false;
    
      Communicator* cellular;
      SerialDebug* usbDebug;
      Locator* positioning;
  };


#endif 
