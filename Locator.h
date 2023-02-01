#ifndef LOCATOR_H
#define LOCATOR_H

#include <TinyGPSPlus.h>

#include "SerialDebug.h"

class Locator
  {
    public:
      Locator(SerialDebug* aUsbDebug);
      byte watchDog();
      void enterPrtMode();
      bool getPos(float* crntLon, float* crntLat);
      bool getPrtPos(float* aPrtLon, float* aPrtLat);
      void setIntervalAlmMode(unsigned int secs);

    private:
      float prtLon;
      float prtLat;
      float lastLon;
      float lastLat;
      SerialDebug* pUsbDebug;
      TinyGPSPlus* gps;
      unsigned int interval;
      unsigned long lastPosTime;
      bool protection_enable = false;
      
      void acquire_nmea_while_ms(unsigned long ms);
      bool waitGPSFix(unsigned long max_wait_sec);
      void rqPos();
  };


#endif
