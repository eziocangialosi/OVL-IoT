/**
 * @file        Tracker.h
 * @brief       Definition of Tracker class
 * @details     This class manage all the algorithm of the tracker (this is the tracker "skeleton") : MQTT answers, Alarm mode, etc...
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef TRACKER_H
#define TRACKER_H

#include "SerialDebug.h"
#include "Locator.h"
#include "Communicator.h"

/**
 * @class       Tracker Tracker.h "Tracker.h"
 * @brief       This class manage all the algorithm of the tracker (this is the tracker "skeleton") : MQTT answers, Alarm mode, etc...
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
class Tracker
  {
    public:
      /** @brief Method to initialize the object
       *  @details This method initialize the object and Comunicator, LedIndicator and SerialDebug */
      void beg();

      /** @brief Method that should be call at each arduino loop turn */
      void actionInLoop();
      
    private:
      /** @brief Method to check the validity of config.h parameters
       *  @warning This method is not already coded */
      bool checkInitParam();

      /** @brief Method to enter in alarm mode
       *  @details This method edit the maximum and minimal interval and send ALM to server */
      void beginAlarm();

      /** @brief Method called when an MQTT message is recived 
       *  @param payload String containing the recived message
       *  @note This method is called by Communicator, it signature/address is given to Communicator in beg() method
       *  @remark This method isn't called when SYN-ACK is recived, this frame is managed in Communicator */
      void whenMqttRx(String payload);

      /** @brief Method used to send the position to the server
       *  @param aLat float with the current latitude
       *  @param aLon float with the current longitude */
      void sendPos(float aLat, float aLon);

      /** @brief Method used to send the tracker status to the server */
      void sendSts();

      /** @brief Method call when the tracker recived an STG frame to acquire parameters
       *  @details This method extract the parameters from the recived frame
       *  @param payload String containing the STG frame */
      void parseParamFrame(String payload); //Method to parse the recived STG frame (Acquire params from Srv)

      /** @brief Method that manages the charge on vehicle battery */
      void veh_charge_manager();


      /** @brief Method call when a position request is recived
       *  @details This method responds the right frame depending of the situation :
       *  If position can be send, it send position, if it cannot send an error frame */
      void mqtt_whenPosRq();

      /** @brief Method call when the server try to change the protection mode value
       *  @details This method responds the right frame depending of the situation 
       *  @remark If the gps fix isn't available the this method will send an "PRT-LIM" frame, this frame
       *  means that the protection mode is limited */
      void mqtt_whenPrt(char aValue);

      /** @brief Method call when a safezone request is recived
       *  @details This method send the safe zone coordinates if protection mode is enabled */
      void mqtt_whenSfzRq();

      bool alarm_is_on = false; //!< If the alarm is on/ringing
      bool waitForParam = false; //!< If we wait for server for parameters
      bool paramSetted = false; //!< If the parameters are set
      bool allowChargeOnVehicle = DEFAULT_VEH_CHG_STS; //!< If the user allow charge on the vehicle battery
      bool eco_mode = DEFAULT_ECO_MODE_STS; //!< If the eco mode is activated (NOT IMPLEMENTED YET)

      bool pos_as_been_rq = false; //!< If the protection mode as been requested by server
      int old_veh_alim_state = LOW; //!< Last vehicle state read on sensor pin
      unsigned long veh_alim_time = 0; //!< Time since which power is detected on sensor pin
      bool isOnCharge = false; //!< If the vehicle is on charge
    
      Communicator* cellular; //!< Pointor to Communicator instance (Srv communication)
      SerialDebug* usbDebug; //!< Pointor to SeriaDebug instance (Debug msg on usb port)
      Locator* positioning; //!< Pointor to Locator instance (GPS communication, acquire and parse)
      LedIndicator* lightSign; //!< Pointor to LedIndicator instance (Debug led)
  };


#endif 
