#include "Tracker.h"

Tracker g_iot;

void setup() {
  g_iot.beg();
}

void loop() {
  g_iot.actionInLoop();
}
