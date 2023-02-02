#ifndef TRACKER_H
#define TRACKER_H

#include "SerialDebug.h"
#include "Locator.h"
#include "Communicator.h"

class Tracker
  {
    public:
      void beg();
      void whenMqttRx(String payload);
      bool actionInLoop();
    private:
      bool checkInitParam();
      void beginAlarm();

      bool alarm_is_on = false;
    
      Communicator* cellular;
      SerialDebug* usbDebug;
      Locator* positioning;
  };


#endif 
