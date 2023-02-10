#ifndef LOCATOR_H
#define LOCATOR_H

#include <TinyGPSPlus.h>

#include "SerialDebug.h"

class Locator
  {
    public:
      Locator(SerialDebug* aUsbDebug); //Constructor, pointor to serial debug class needed to send debug info
      void beg(); //Intialization method
      byte watchDog(); //Method who check if the gps position has changed
      void enterPrtMode(); //Method to enter in protection mode
      bool getPos(float* crntLat, float* crntLon); //Getter for the current position (Lat, Lon), return 1 if success, 0 if error
      bool getPrtPos(float* aPrtLat, float* aPrtLon); //Getter for the protection position 
      void setInterval(unsigned int secs); //Setter for the maximum interval between 2 position refresh
      bool getIsInit(); //Getter for the is init flag
      void setSafeZoneDiam(unsigned int aDiam); //Setter for the safe zone diameter (diameter in meters)
      bool isProtectionEnable(); //Getter, true if the protecion is enabled false if isn't
      void quitPrtMode(); //Method to quit the protection mode
      bool gpsIsFixed(); //Getter, true if the GPS has a fix false if isn't

    private:
      float prtLon; //Protection position (Lon)
      float prtLat; //same (Lat)
      float lastLon; //Last recvied position (Lon)
      float lastLat; //same (Lat)
      bool isInit = false; //If the object is intialize or not
      unsigned int safeZoneDiam = 5; //Safe zone diameter
      SerialDebug* pUsbDebug; //Pointor to serial debug obj
      TinyGPSPlus* gps; //Pointor to GPS obj (GPS Module)
      unsigned int interval; //Interval between 2 position refresh
      unsigned long lastPosTime; //"Timestamp" of the last position refresh
      bool protection_enable = false; //If the proection is enable
      
      void acquire_nmea_while_ms(unsigned long ms); //Wait a serial entry for milliseconds
      bool waitGPSFix(unsigned long max_wait_sec); //Wait for a GPS Fix while secondes
      void rqPos(); //Actualize position stored with last position recived from module
      float round_float_dp(float in_value, int decimal_place); //Round float number for x decimals
  };


#endif
