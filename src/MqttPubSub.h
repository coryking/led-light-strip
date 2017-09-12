//
// Created by Cory King on 9/12/17.
//

#ifndef RGBWPLAY_MQTTPUBSUB_H
#define RGBWPLAY_MQTTPUBSUB_H

#include <Arduino.h>

#include <FastLED.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"

#define MQTT_GET_POWER "/lightbulb/power/get"
#define MQTT_GET_HUE "/lightbulb/hue/get"
#define MQTT_GET_BRIGHTNESS "/lightbulb/brightness/get"
#define MQTT_GET_SATURATION "/lightbulb/saturation/get"

#define MQTT_SET_POWER "/lightbulb/power"
#define MQTT_SET_HUE "/lightbulb/hue"
#define MQTT_SET_BRIGHTNESS "/lightbulb/brightness"
#define MQTT_SET_SATURATION "/lightbulb/saturation"

class MqttPubSub {

public:
    typedef std::function<void(void)> ReconnectCallback;
    typedef std::function<void(int)> IntValueCallback;
    typedef std::function<void(bool)> PowerCallback;

    typedef std::function<bool(void)> GetPowerCallback;
    typedef std::function<CHSV(void)> GetHSVCallback;
    typedef std::function<uint8_t(void)> GetBrightnessCallback;

    MqttPubSub(WiFiClient& wifiClient) {
        char tmpHostString[16] = {0};
        sprintf(tmpHostString, "ESP_%06X", ESP.getChipId());
        espHostString = tmpHostString;

        this->client = PubSubClient(wifiClient);
        client.setServer(MQTT_SERVER, 1883);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            this->mqttCallback(topic,payload,length);
        });

        mqttGetBrightnessTopic = espHostString + MQTT_GET_BRIGHTNESS;
        mqttGetHueTopic = espHostString + MQTT_GET_HUE;
        mqttGetSaturationTopic = espHostString + MQTT_GET_SATURATION;
        mqttGetPowerTopic = espHostString + MQTT_GET_POWER;

        mqttSetBrightnessTopic = espHostString + MQTT_SET_BRIGHTNESS;
        mqttSetSaturationTopic = espHostString + MQTT_SET_SATURATION;
        mqttSetHueTopic = espHostString + MQTT_SET_HUE;
        mqttSetPowerTopic = espHostString + MQTT_SET_POWER;

    }

    void publishHSV(const CHSV &hsv);

    void publishBrightness(const uint8_t brightness);

    void publishPower(const bool isOn);

    MqttPubSub* setLightSaturationCallback(const IntValueCallback lightSaturationCallback);

    MqttPubSub* setLightBrightnessCallback(const IntValueCallback lightBrightnessCallback);

    MqttPubSub* setLightHueCallback(const IntValueCallback lightHueCallback);

    MqttPubSub* setLightPowerCallback(const PowerCallback lightPowerCallback);

    void setReconnectCallback(const ReconnectCallback reconnectCallback);

    void connect();
    void loop();
private:
    PubSubClient client;
    ulong lastReconnectAttempt = 0;
    void setSubscriptions();
    bool reconnect();

    String espHostString;

    String mqttGetPowerTopic;
    String mqttGetHueTopic;
    String mqttGetBrightnessTopic;
    String mqttGetSaturationTopic;

    String mqttSetPowerTopic;
    String mqttSetHueTopic;
    String mqttSetBrightnessTopic;
    String mqttSetSaturationTopic;

    IntValueCallback lightSaturationCallback;
    IntValueCallback lightBrightnessCallback;
    IntValueCallback lightHueCallback;
    PowerCallback lightPowerCallback;

    ReconnectCallback reconnectCallback;

    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void setPayloadToIntCb(const MqttPubSub::IntValueCallback cb, byte *payload, unsigned int length, int ol, int oh);
    void intRangeToChar(uint16_t in, uint16_t il, uint16_t ih, uint16_t ol, uint16_t oh, char* outBuff);
    void publish(String &topic, const char* value);
    void subscribe(String& topic);


};

#endif //RGBWPLAY_MQTTPUBSUB_H
