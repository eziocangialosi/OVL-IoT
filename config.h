#ifndef CONFIG_H
#define CONFIG_H

/*******************************************/
// GPRS config
#define GSM_BAUD 9600 //Serial baud used ot communicate with gprs module

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

#define GSM_PIN "####" //SIM card pin
const char APN[]      = "example.com"; //URL of your phone operator apn
const char GPRS_USER[] = ""; //Your gprs user (if needed)
const char GPRS_PSWD[] = ""; //Your gprs password (if needed)


/*******************************************/
// MQTT details
#define MQTT_PORT 6868 //Broker port
const char BROKER[]    = "example.com"; //Broker URL or IP addr
const char TOPIC_RX[]  = "<iot_id>RX"; //Rx Topic used for this tracker
const char TOPIC_TX[]  = "<iot_id>TX"; //Tx Topic used for this tracker
const char MQTT_USER[] = "mqtt_user"; //Mqtt user for this tracker
const char MQTT_PSWD[] = "mqtt_pass"; //Mqtt password for this tracker


/*******************************************/
// GPS config
#define GPS_BAUD 9600 //Serial baud used for gps module
#define DP_POS 3 //Number of digits used to compare 2 positions, defaut 3


/*******************************************/
//Tracker misc params
#define DEFAULT_VEH_CHG_STS false //If the tracker can charge on vehicle battery by default
#define DEFAULT_ECO_MODE_STS false //If the tracker should be in ecomode by default
#define DEFAULT_SAFE_ZONE_DIAM 5 //Default diameter of the SafeZone, default 5
#define DEFAULT_INTERVAL (5*60) //Default interval between 2 position refresh, default 5min
#define DEFAULT_INTERVAL_WHEN_ALM 5 //Default interval between 2 position refresh when alarm is on, default 5sec

/*******************************************/
//Debug config
#define USB_BAUD 115200 //Serial baud used to communicate with debug console

#endif