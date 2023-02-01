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
