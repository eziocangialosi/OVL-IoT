/**
 * @file        Locator.h
 * @brief       Definition of Locator class
 * @details     This class manage positioning system (GPS)
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef LOCATOR_H
#define LOCATOR_H

#include <TinyGPSPlus.h>

#include "SerialDebug.h"
#include "LedIndicator.h"

/**
 * @class       Locator Locator.h "Locator.h"
 * @brief       This class manage call the positioning stuff
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
class Locator
  {
    public:
      /** @brief Constructor
       *  @details This constructor NOT initialize the class, it just usefull to give SerialDebug and LedIndicator object address
       *  @param aUsbDebug pointor to SerialDebug class needed to send debug info
       *  @param apLightSign pointor to LedIndicator needed to debug via led codes
       *  @warning This class should not be use without intialize it with beg() method */
      Locator(SerialDebug* aUsbDebug, LedIndicator* apLightSign);

      /** @brief Intialization method
       *  @details This method intilize the class. It wait for the GPS fix and setting up parameters */
      void beg();

      /** @brief Position algorithm
       *  @details This method decide if the position has changed or not and if the position hasn't be send for too long
       *  @return Return a byte, if the first bit (LSB) is true, the position need to be send,
       *  if the 2nd bit is true, the tracker is outside the safezone
       *  @pre Before using this method, please initialize the class */
      byte watchDog();
      
      /** @brief Used to enter in protection mode
       *  @details This method will define the safezone center at the current position
       *  @note If the position cannot be established, the method will set the safezone at the last known position */
      void enterPrtMode();

      /** @brief Getter for the current position
       *  @param crntLat pointor to a float where the latitude will be write
       *  @param crntLon pointor to a float where the longitude will be write
       *  @return True if the position could be collected, false otherwise */
      bool getPos(float* crntLat, float* crntLon);

      /** @brief Getter for the protection position
       *  @param aPrtLat pointor to a float where the latitude will be write
       *  @param aPrtLon pointor to a float where the longitude will be write
       *  @return True if protection mode is enabled, false if isn't or if an error occured
       *  @note The protection position is the safezone center */
      bool getPrtPos(float* aPrtLat, float* aPrtLon);

      /** @brief Setter for the MAXIMUM interval between two positions refresh/send
       *  @param secs interval in seconds 
       *  @note The maximum interval is used to force updating the position even if aren't movement */
      void setInterval(unsigned int secs);

      /** @brief Setter for the MINIMAL interval between two positions refresh/send
       *  @param secs interval in seconds 
       *  @note The minimal interval is used to prevent too regular updating of the position */
      void setMinimalInterval(unsigned int secs);

      /** @brief Getter to know if the class was initialize or not
       *  @return True if the class is init, false otherwise */
      bool getIsInit();

      /** @brief Setter for safezone diameter
       *  @param aDiam safezone diameter in meters */
      void setSafeZoneDiam(unsigned int aDiam);

      /** @brief Getter to know if the protection mode is enabled or not
       *  @return True if it is, false otherwise */
      bool isProtectionEnable();

      /** @brief Method to quit the protection mode
       *  @pre The protection mode should be enabled */
      void quitPrtMode();

      /** @brief Getter to know if the gps is fix
       *  @param True if the GPS has fix, false if hasn't
       *  @note GPS fix --> position avail */
      bool gpsIsFixed();

    private:
      float prtLon; //Protection position (Lon)
      float prtLat; //same (Lat)
      float lastLon; //Last recvied position (Lon)
      float lastLat; //same (Lat)
      bool isInit = false; //If the object is intialize or not
      unsigned int safeZoneDiam = DEFAULT_SAFE_ZONE_DIAM; //Safe zone diameter
      SerialDebug* pUsbDebug; //Pointor to serial debug obj
      TinyGPSPlus* gps; //Pointor to GPS obj (GPS Module)
      LedIndicator* pLightSign; //Pointor to led debug obj
      unsigned int interval; //Max interval between 2 position refresh
      unsigned int min_interval; //Minimal interval between 2 position refresh
      unsigned long lastPosTime; //"Timestamp" of the last position refresh
      bool protection_enable = false; //If the proection is enable
      
      void acquire_nmea_while_ms(unsigned long ms); //Wait a serial entry for milliseconds
      bool waitGPSFix(unsigned long max_wait_sec); //Wait for a GPS Fix while secondes
      void rqPos(); //Actualize position stored with last position recived from module
      float round_float_dp(float in_value, int decimal_place); //Round float number for x decimals
  };


#endif
