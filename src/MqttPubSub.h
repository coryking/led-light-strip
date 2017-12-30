//
// Created by Cory King on 9/12/17.
//

#ifndef RGBWPLAY_MQTTPUBSUB_H
#define RGBWPLAY_MQTTPUBSUB_H

#include <Arduino.h>

#include <FastLED.h>

#include <Task.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"
#include <MqttController.h>

#define MQTT_GET_RANDOM "/lightbulb/random/get"
#define MQTT_GET_POWER "/lightbulb/power/get"
#define MQTT_GET_HUE "/lightbulb/hue/get"
#define MQTT_GET_BRIGHTNESS "/lightbulb/brightness/get"
#define MQTT_GET_SATURATION "/lightbulb/saturation/get"

#define MQTT_SET_RANDOM "/lightbulb/random"
#define MQTT_SET_POWER "/lightbulb/power"
#define MQTT_SET_HUE "/lightbulb/hue"
#define MQTT_SET_BRIGHTNESS "/lightbulb/brightness"
#define MQTT_SET_SATURATION "/lightbulb/saturation"

// how long to wait for MQTT to settle before trying to connect
#define MQTT_CONNECT_WAIT 6000

class MqttPubSub : public MqttController {

public:
    typedef std::function<void(int)> IntValueCallback;
    typedef std::function<void(bool)> PowerCallback;

    MqttPubSub(const char* mqttHost, const uint16_t mqttPort) : MqttController(mqttHost,mqttPort, &syslog) {
        char espTheTopicId[20] = {0};
        sprintf(espTheTopicId, "ESP_%06X", ESP.getChipId());

        String espHostString((const char *)espTheTopicId);

        mqttGetBrightnessTopic = espHostString + MQTT_GET_BRIGHTNESS;
        mqttGetHueTopic = espHostString + MQTT_GET_HUE;
        mqttGetSaturationTopic = espHostString + MQTT_GET_SATURATION;
        mqttGetPowerTopic = espHostString + MQTT_GET_POWER;
        mqttGetRandomTopic = espHostString + MQTT_GET_RANDOM;


        mqttSetBrightnessTopic = espHostString + MQTT_SET_BRIGHTNESS;
        mqttSetSaturationTopic = espHostString + MQTT_SET_SATURATION;
        mqttSetHueTopic = espHostString + MQTT_SET_HUE;
        mqttSetPowerTopic = espHostString + MQTT_SET_POWER;
        mqttSetRandomTopic = espHostString + MQTT_SET_RANDOM;


    }

    void publishHSV(const CHSV &hsv);

    void publishBrightness(const uint8_t brightness);

    void publishPower(const bool isOn);

    void publishRandom(const bool isRandomMode);

    MqttPubSub* setLightSaturationCallback(const IntValueCallback lightSaturationCallback);

    MqttPubSub* setLightBrightnessCallback(const IntValueCallback lightBrightnessCallback);

    MqttPubSub* setLightHueCallback(const IntValueCallback lightHueCallback);

    MqttPubSub* setLightPowerCallback(const PowerCallback lightPowerCallback);

    MqttPubSub* setLightRandomCallback(const PowerCallback lightRandomCallback);

protected:
    void mqttCallback(char *topic, byte *payload, unsigned int length) override;
    void setSubscriptions() override;

private:

    String mqttGetRandomTopic;
    String mqttGetPowerTopic;
    String mqttGetHueTopic;
    String mqttGetBrightnessTopic;
    String mqttGetSaturationTopic;

    String mqttSetRandomTopic;
    String mqttSetPowerTopic;
    String mqttSetHueTopic;
    String mqttSetBrightnessTopic;
    String mqttSetSaturationTopic;

    IntValueCallback lightSaturationCallback;
    IntValueCallback lightBrightnessCallback;
    IntValueCallback lightHueCallback;
    PowerCallback lightPowerCallback;
    PowerCallback lightRandomCallback;



    void setPayloadToIntCb(const MqttPubSub::IntValueCallback cb, const byte *payload, unsigned int length, int ol, int oh);
    void intRangeToChar(uint16_t in, uint16_t il, uint16_t ih, uint16_t ol, uint16_t oh, char* outBuff);

};

#endif //RGBWPLAY_MQTTPUBSUB_H
