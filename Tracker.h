#ifndef TRACKER_H
#define TRACKER_H

#include "SerialDebug.h"
#include "Locator.h"

class Tracker
  {
    public:
      Tracker();
      void whenMqttRx(String payload);

    private:
      bool checkInitParam();
      void beginAlarm();
    
      //Communicator* cellular;
      SerialDebug* usbDebug;
      Locator* positioning;
  };


#endif 
