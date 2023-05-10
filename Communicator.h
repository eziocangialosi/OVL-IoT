/**
 * @file        Communicator.h
 * @brief       Definition of Communicator class
 * @details     This class manage everything related to communication (GPRS & MQTT)
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#define TRACKER_CALLBACK_SIG std::function<void(String)> calledWhenMsg

#include "SerialDebug.h"
#include "LedIndicator.h"

#include <PubSubClient.h>
#include <TinyGsmClient.h>

/**
 * @class       Communicator Communicator.h "Communicator.h"
 * @brief       Class that managing everything related to comunication
 * @details     This one is used to manage the dialog with the GPRS module and the implementation of MQTT protocol
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
class Communicator
  {
    public:
      /** @brief Constructor, pointor to serial debug class needed to send debug info, pointor to led sign needed to debug via led codes
       *  @param apSerialDebug pointor to SerialDebug class needed to send debug info
       *  @param apLightSign pointor to LedIndicator needed to debug via led codes */
      Communicator(SerialDebug* apSerialDebug, LedIndicator* apLightSign);
      
      /** @brief Method to (Re)unlock sim card
       * @ return True if the sim card has been unlocked, false otherwise */
      bool unlockSIM();
      
      /** @brief Method to (Re)connect GPRS
       *  @return True if the gprs is now connected, false otherwise */
      bool connectGPRS();
      
      /** @brief Method to (Re)connect to MQTT broker
       *  @return True if the mqtt connection is established, false otherwise */
      bool connectMQTT();
      
      /** @brief Method to send an MQTT message on the TX topic
       *  @param aFrame String containing the frame to send
       *  @return True if the message has been sent, false otherwise */
      bool sendMqtt(String aFrame);
      
      /** @brief Getter to know if the connection is established with server
       *  @note Connection is considered established when MQTT, GPRS and handshake as success
       *  @return True if the connection is established, false otherwise */
      bool getIsConnected();

      /** @brief Getter to know we're currently waiting for the handshake anwser
       *  @return True if the tracker waiting for an answer, false otherwise */
      bool getWaitingForHandshake();

      /** @brief Getter of the timestamp of the last handshake attempt
       *  @note The timestamp is in milliseconds and does not represent UTC datetime
       *  @return Timestamp in milliseconds */
      unsigned long getLastHandshakeRq();
      
      /** @brief Getter to now is the handshake has success
       *  @note Handshake is successful when tracker recive "SYN-ACK"
       *  @return True if the handshake has success, false otherwise */
      bool getHandshakeSuccess();

      /** @brief Method to call the mqtt loop (Mandatory to recived mqtt messages)
       *  @note This loop should be called regularly to allow the client to process incoming messages and maintain its connection to the server */
      void execMqttLoop();

      /** @brief Method that handle the handshake answer
       *  @param String that contain the recived message */
      void handshakeHandle(String msg);

      /** @brief Setter to define the method called when a mqtt message is recived
       *  @note TRACKER_CALLBACK_SIG is an address of an void function that take a String in parameter
       *  @param TRACKER_CALLBACK_SIG address of a method that can handle mqtt messages */
      void setCallWhenMsg(TRACKER_CALLBACK_SIG);

      /** @brief Getter of the battery percentage
       *  @warning This percentage is taken from the GPRS module, it can be not reliable
       *  @return Percentage of the battery beetween 0 to 100% */      
      unsigned int getBatteryPercent();

      /** @brief Getter of the battery voltage
       *  @warning This percentage is taken from the GPRS module, it can be not reliable
       *  @return Battery voltage in millivolts */   
      unsigned int getBatteryVolt();

      /** @brief Getter to know if the battery is in charge
       *  @warning This information is taken from the GPRS module, it can be not reliable
       *  @return True if the battery is in charge, false otherwise */   
      bool getBatteryInCharge();
      
      /** @brief Method called when connection is lost, it tries to restore the connection
       *  @note It will try to restore GPRS and/or MQTT connection, it cannot retry to unlock the sim card */   
      void autoReconnect();

      /** @brief Method used to try an handshake with server */
      void tryHandshake();

      /** @brief Getter to know if the sim card is locked
       *  @warning This information is taken from the GPRS module, it can be not reliable
       *  @return True if the sim card is locked, false otherwise */  
      bool getSimLocked();

      /** @brief Method to (re)connect the cellular network
       *  @return True if the connection succeeded, false otherwise */
      bool connectNetwork();

      /** @brief Method to send a debug MQTT message on the TX topic
       *  @param aPayload String containing the message to send
       *  @note This method use sendMqtt() method but adds TXT frame syntax
       *  @return True if the message has been sent, false otherwise */
      bool sendDebugMqtt(String aPayload);
      
    private:
      SerialDebug* pUsbDebug; //!< Pointor to SerialDebug class
      bool waitingForHandshake = false; //!< If we wait for the Handshake validation
      bool handshakeSuccess = false; //!< If the Handshake as success
      bool wasInit = false; //!< If the obj was initialize
      bool isFirstHandshake = true; //!< If there was more than one handshake
      unsigned long lastHandshakeRq = 0; //!< Timestamp of the last handshake request
      uint32_t lastReconnectAttempt = 0; //!< Timestamp of the last reconnect attempt
      TinyGsm* pModem; //!< Pontor to TinyGsm class (GSM/GPRS Module)
      TinyGsmClient* pClient; //!< Pointor to TinyGsmClient class (Interface between MQTT and GSM/GPRS module)
      PubSubClient*  pMqtt; //!< Pointor to PubSubClient class (MQTT Client)
      LedIndicator* pLightSign; //!< Pointor to led debug obj
      TRACKER_CALLBACK_SIG; //!< Callback function stored

      /** @brief Method called when a mqtt message is recvied
       *  @param topic char array with the topic name
       *  @param payload byte array containing the MQTT message
       *  @param len unsigned int containing the payload lenght */
      void mqttCallback(char* topic, byte* payload, unsigned int len); //Method called when a mqtt message is recvied
  };

#endif 
