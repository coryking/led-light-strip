//
// Created by Cory King on 9/12/17.
//

#ifndef RGBWPLAY_CONFIG_H
#define RGBWPLAY_CONFIG_H

#include <Syslog.h>

#define YOUR_WIFI_SSID "Cats Drinking Wine"
#define YOUR_WIFI_PASSWD "catsmeow"

#define MQTT_SERVER "mqtt.gotokingfamily.com"
#define MQTT_PORT 1883

// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.85"
#define SYSLOG_PORT 514

// This device info
#define APP_NAME "led-controller"

extern Syslog syslog;

#endif //RGBWPLAY_CONFIG_H
