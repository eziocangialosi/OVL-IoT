/**
 * @file        OVL-IoT.ino
 * @brief       Arduino loop & setup and instance creation of the tracker object
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
 
#include "Tracker.h"

Tracker g_iot; //!< Instance of Tracker class

void setup() {
  g_iot.beg();
}

void loop() {
  g_iot.actionInLoop();
}
