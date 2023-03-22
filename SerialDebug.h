#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "Arduino.h"
#include "config.h"

#define MSG_BORDER  "============================================"
#define VERSION_MSG "OpenVehicleLocator Pre-Developpement version"

class SerialDebug
  {
    public:
      SerialDebug(unsigned int aBaud); //Constructor, aBaud -> serial baud of serial link
      bool wrt(String payload); //Write on serial and go next line at end, return true if success, false if failed
      bool wrt_inline(String payload); //Write on serial on the same line, return true if success, false if failed
      
    private:
      unsigned int baud; //Serial baud of the link
  };

#endif
