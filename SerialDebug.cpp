/**
 * @file        SerialDebug.cpp
 * @brief       All methods code for SerialDebug class
 * @details     Methods code for usb debug port
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     v0.8.0-alpha
 * @date        04/2023
 */

#include "SerialDebug.h"

SerialDebug::SerialDebug(unsigned int aBaud){
  this->baud = aBaud;
  Serial.begin(aBaud);
  Serial.println(MSG_BORDER);
  Serial.println(VERSION_MSG);
  Serial.println(MSG_BORDER);
}

bool SerialDebug::wrt(String payload){
  return Serial.println(payload);
}

bool SerialDebug::wrt_inline(String payload){
  return Serial.print(payload);
}
