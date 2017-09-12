//
// Created by Cory King on 9/12/17.
//

#ifndef RGBWPLAY_CONFIG_H
#define RGBWPLAY_CONFIG_H

#define YOUR_WIFI_SSID "Cats Drinking Wine"
#define YOUR_WIFI_PASSWD "catsmeow"

#define MQTT_SERVER "192.168.3.3"

typedef enum {
    POWERING_ON,
    POWER_ON,
    POWERING_OFF,
    POWERED_OFF,

} PowerState;

#endif //RGBWPLAY_CONFIG_H
