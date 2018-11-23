//
// Created by Cory King on 9/12/17.
//

#ifndef RGBWPLAY_WIFI_H_H
#define RGBWPLAY_WIFI_H_H


#include <functional>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"

void reconnectWiFi() {
    Serial.println("Attempting to reconnect to Wifi....");
    WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);
}

void setupWiFi(const char *theHostName) {
    static WiFiEventHandler e1, e2;

    WiFi.mode(WIFI_STA);
    WiFi.hostname(theHostName);

    //onSyncEvent_t onSyncHandler = onNTPSyncEvent;
    //NTP.onNTPSyncEvent(onSyncHandler);
    std::function<void(const WiFiEventStationModeGotIP&)> handler = [](WiFiEventStationModeGotIP ipInfo) {
        Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
    };
    e1 = WiFi.onStationModeGotIP(handler);// As soon WiFi is connected, start NTP Client
    WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

    Serial.print("Waiting for Wifi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    IPAddress ip = WiFi.dnsIP(0);
    Serial.println(ip.toString());
    WiFi.printDiag(Serial);
    Serial.println("");
    syslog.log(LOG_WARNING, "Connected to wifi....");
    Serial.println("WiFi connected");
}

#endif //RGBWPLAY_WIFI_H_H
