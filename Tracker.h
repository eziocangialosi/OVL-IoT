#ifndef TRACKER_H
#define TRACKER_H

#include "SerialDebug.h"
#include "Locator.h"
#include "Communicator.h"

class Tracker
  {
    public:
      void beg();//Intialize method
      void actionInLoop();//Method called at each loop turn 
    private:
      bool checkInitParam(); //Method to check the validity of config.h params
      void beginAlarm(); //Method to begin the alarm mode
      void whenMqttRx(String payload); //Method called by Communicator instance when a new MQTT frame is recived
      void sendPos(float aLat, float aLon); //Method to send the position to the Svr
      void sendSts(); //Method to send tracker status to the Srv
      void parseParamFrame(String payload); //Method to parse the recived STG frame (Acquire params from Srv)

      bool alarm_is_on = false; //If the alarm is on/ringing
      bool waitForParam = false; //If we wait for server for parameters
      bool paramSetted = false; //If the parameters are set
      bool allowChargeOnVehicle = false; //If the user allow Charge on the vehicle battery
      bool eco_mode = false; //If the eco mode is activated

      bool prt_as_been_rq = false; //If the protection mode as been requested by Srv
    
      Communicator* cellular; //Pointor to Communicator instance (Srv communication)
      SerialDebug* usbDebug; //Pointor to SeriaDebug instance (Debug msg on usb port)
      Locator* positioning; //Pointor to Locator instance (GPS communication, acquire and parse)
  };


#endif 
