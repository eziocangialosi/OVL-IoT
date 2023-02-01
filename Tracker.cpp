#include "Tracker.h"

Tracker::Tracker(){
  this->usbDebug = new SerialDebug(115200);
  this->positioning = new Locator(usbDebug);
}
