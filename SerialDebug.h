/**
 * @file        SerialDebug.h
 * @brief       Definition of SerialDebug class
 * @details     This class is used to send debug data via the USB port of the firebeetle
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "Arduino.h"
#include "config.h"

//!< Messages will be send on serial when tracker boot
#define MSG_BORDER  "============================================"
#define VERSION_MSG "OpenVehicleLocator Pre-Developpement version"

class SerialDebug
  {
    public:
      /** @brief Constructor of SerialDebug class
       *  @param aBaud baud of the UART link */
      SerialDebug(unsigned int aBaud);

      /** @brief Method to write on serial and go next line at the end
       *  @param payload the message to send on serial
       *  @return True if success, false otherwise 
       *  @note  To send a message on serial without go next line, use wrt_inline() */
      bool wrt(String payload);

      /** @brief Method to write on serial on the same line
       *  @param payload the message to send on serial
       *  @return True if success, false otherwise 
       *  @note  To send a message on serial and go next line, use wrt() */
      bool wrt_inline(String payload);
      
    private:
      unsigned int baud; //!< Serial baud of uart link
  };

#endif
