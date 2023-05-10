/**
 * @file        config.h
 * @brief       All the tracker configuration
 * @details     Here you will find all the configuration, if you require to edit a parameter do it here
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef CONFIG_H
#define CONFIG_H

/*******************************************/
// GPRS config

/**
 * @def GSM_BAUD
 * Serial baud used ot communicate with gprs module
 */
#define GSM_BAUD 9600 

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM7000
// #define TINY_GSM_MODEM_SIM7000SSL
// #define TINY_GSM_MODEM_SIM7080
// #define TINY_GSM_MODEM_SIM5360
// #define TINY_GSM_MODEM_SIM7600
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_SARAR4
// #define TINY_GSM_MODEM_M95
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_MC60
// #define TINY_GSM_MODEM_MC60E
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE
// #define TINY_GSM_MODEM_SEQUANS_MONARCH

/**
 * @def GSM_PIN
 * SIM card pin
 */
#define GSM_PIN ""

/**
 * @var const char APN
 * URL of your mobile operator apn
 */
const char APN[]      = "example.com";

/**
 * @var const char GPRS_USER
 * Your gprs user (if needed)
 */
const char GPRS_USER[] = "";

/**
 * @var const char GPRS_PSWD
 * Your gprs password (if needed)
 */
const char GPRS_PSWD[] = "";


/*******************************************/
// MQTT details

/**
 * @def MQTT_PORT
 * Broker port
 */
#define MQTT_PORT 1883

/**
 * @var const char BROKER
 * Broker URL or IP addr
 */
const char BROKER[]    = "example.com";

/**
 * @var const char TOPIC_RX
 * Rx Topic used for this tracker
 */
const char TOPIC_RX[]  = "topicRX";

/**
 * @var const char TOPIC_TX
 * Tx Topic used for this tracker
 */
const char TOPIC_TX[]  = "topicTX";

/**
 * @var const char MQTT_USER
 * Mqtt user for this tracker
 */
const char MQTT_USER[] = "mqtt_user";

/**
 * @var const char MQTT_PSWD
 * Mqtt password for this tracker
 */
const char MQTT_PSWD[] = "mqtt_pass";


/*******************************************/
// GPS config

/**
 * @def GPS_BAUD
 * Serial baud used for gps module
 */
#define GPS_BAUD 9600

/**
 * @def DISTANCE_TRIG
 * Distance from which we define that the position has changed, default 5 meters
 */
#define DISTANCE_TRIG 5


/*******************************************/
//Tracker misc params

/**
 * @def DEFAULT_VEH_CHG_STS
 * If the tracker can charge on vehicle battery by default
 */
#define DEFAULT_VEH_CHG_STS false

/**
 * @def DEFAULT_ECO_MODE_STS
 * If the tracker should be in ecomode by default
 */
#define DEFAULT_ECO_MODE_STS false

/**
 * @def DEFAULT_SAFE_ZONE_DIAM
 * Default diameter of the SafeZone, default 15
 */
#define DEFAULT_SAFE_ZONE_DIAM 15

/**
 * @def DEFAULT_INTERVAL
 * Default interval between 2 position refresh, default 5min
 */
#define DEFAULT_INTERVAL (5*60)

/**
 * @def DEFAULT_INTERVAL_WHEN_ALM
 * Default interval between 2 position refresh when alarm is on, default 5sec
 */
#define DEFAULT_INTERVAL_WHEN_ALM 5

/**
 * @def MINIMAL_INTERVAL
 * Minimal interval beetween 2 position refresh when alarm is off, default 30sec
 */
#define MINIMAL_INTERVAL 30

/**
 * @def DELAY_CHG_VEH
 * Default delay before charging on vehicle battery when is avail, default 5min
 */
#define DELAY_CHG_VEH (5*60)

/**
 * @def VEH_ALIM_SENSOR_PIN
 * Pin used to detect when the vehicle power is avail
 */
#define VEH_ALIM_SENSOR_PIN 4

/**
 * @def VEH_ALIM_RELAY_PIN
 * Pin used to command the charge on vehicle battery
 */
#define VEH_ALIM_RELAY_PIN 12

/**
 * @def LOW_VOLTAGE
 * Voltage in millivolts at which it is considered that the battery can be damaged (default 3500mV)
 */
#define LOW_VOLTAGE 3500

/*******************************************/
//Debug config

/**
 * @def USB_BAUD
 * Serial baud used to communicate with debug console
 */
#define USB_BAUD 115200

#endif
