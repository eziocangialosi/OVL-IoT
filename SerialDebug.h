#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "Arduino.h"

#define MSG_BORDER  "=================================="
#define VERSION_MSG "OpenVehiculeLocator ProofOfConcept"

class SerialDebug
  {
    public:
      SerialDebug(unsigned int aBaud);
      bool wrt(String payload);
      bool wrt_inline(String payload);
      
    private:
      unsigned int baud;
  };

#endif
