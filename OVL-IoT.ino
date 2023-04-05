/**
 * @file        OVL-IoT.ino
 * @brief       Instance of the tracker object and arduino loop/setup
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
 
#include "Tracker.h"

Tracker g_iot;

void setup() {
  g_iot.beg();
}

void loop() {
  g_iot.actionInLoop();
}
