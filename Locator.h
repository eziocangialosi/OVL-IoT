#ifndef LOCATOR_H
#define LOCATOR_H

#include <TinyGPSPlus.h>

#include "SerialDebug.h"

class Locator
  {
    public:
      Locator(SerialDebug* aUsbDebug);
      void beg();
      byte watchDog();
      void enterPrtMode();
      bool getPos(float* crntLon, float* crntLat);
      bool getPrtPos(float* aPrtLon, float* aPrtLat);
      void setInterval(unsigned int secs);
      bool getIsInit();
      void setSafeZoneDiam(unsigned int aDiam);
      bool isProtectionEnable();
      void quitPrtMode();

    private:
      float prtLon;
      float prtLat;
      float lastLon;
      float lastLat;
      bool isInit = false;
      unsigned int safeZoneDiam = 5;
      SerialDebug* pUsbDebug;
      TinyGPSPlus* gps;
      unsigned int interval;
      unsigned long lastPosTime;
      bool protection_enable = false;
      
      void acquire_nmea_while_ms(unsigned long ms);
      bool waitGPSFix(unsigned long max_wait_sec);
      void rqPos();
      float round_float_dp(float in_value, int decimal_place);
  };


#endif
