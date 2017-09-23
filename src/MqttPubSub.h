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

#define MQTT_GET_POWER "/lightbulb/power/get"
#define MQTT_GET_HUE "/lightbulb/hue/get"
#define MQTT_GET_BRIGHTNESS "/lightbulb/brightness/get"
#define MQTT_GET_SATURATION "/lightbulb/saturation/get"

#define MQTT_SET_POWER "/lightbulb/power"
#define MQTT_SET_HUE "/lightbulb/hue"
#define MQTT_SET_BRIGHTNESS "/lightbulb/brightness"
#define MQTT_SET_SATURATION "/lightbulb/saturation"

// how long to wait for MQTT to settle before trying to connect
#define MQTT_CONNECT_WAIT 6000

enum MQTT_STATE {
    Disconnecting,
    Disconnected,
    Waiting,
    Connecting,
    Connected
};

class MqttPubSub : public Task {

public:
    typedef std::function<void(void)> ReconnectCallback;
    typedef std::function<void(int)> IntValueCallback;
    typedef std::function<void(bool)> PowerCallback;

    typedef std::function<bool(void)> GetPowerCallback;
    typedef std::function<CHSV(void)> GetHSVCallback;
    typedef std::function<uint8_t(void)> GetBrightnessCallback;

    MqttPubSub() : Task(MsToTaskTime(100)) {
        getRandomClientId();

        String espHostString = espTopicId;

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

    virtual void OnUpdate(uint32_t deltaTime);

private:
    PubSubClient *client;
    WiFiClient wifiClient;
    ulong timeWaitStarted = 0;
    void setSubscriptions();
    char espClientId[20] = {0};
    char espTopicId[20] = {0};
    void getRandomClientId();
    void makePubSubClient();
    MQTT_STATE mqttState = Disconnected;

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
