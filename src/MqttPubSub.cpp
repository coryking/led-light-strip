//
// Created by Cory King on 9/12/17.
//

#include "MqttPubSub.h"


void MqttPubSub::mqttCallback(char *topic, byte *payload, unsigned int length) {

    syslog.logf(LOG_INFO, "Message arrived [%s]\n", topic);

    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();

    String theTopic = String(topic);

    if (theTopic == mqttSetHueTopic) {
        setPayloadToIntCb(lightHueCallback, payload, length, 0,360);
    }
    if (theTopic == mqttSetSaturationTopic) {
        setPayloadToIntCb(lightSaturationCallback, payload, length, 0,100);
    }
    if (theTopic == mqttSetBrightnessTopic) {
        setPayloadToIntCb(lightBrightnessCallback, payload, length, 0, 100);
    }
    if (theTopic == mqttSetRandomTopic && lightRandomCallback != NULL) {
        if ( length > 0 && payload[0] == 't') {
            lightRandomCallback(true);
        } else {
            lightRandomCallback(false);
        }
    }
    if (theTopic == mqttSetPowerTopic && lightPowerCallback != NULL) {
        if ( length > 0 && payload[0] == 't') {
            lightPowerCallback(true);
        } else {
            lightPowerCallback(false);
        }
    }
}

void MqttPubSub::publishPower(const bool isOn) {
    publish(mqttGetPowerTopic, isOn ? "true" : "false");
}

void MqttPubSub::publishBrightness(const uint8_t brightness) {
    char outBuff[20] = {0};
    intRangeToChar(brightness, 0, 255, 0, 100, outBuff);
    publish(mqttGetBrightnessTopic, outBuff);
}

void MqttPubSub::publishHSV(const CHSV &hsv) {
    char outBuff[10] = {0};
    intRangeToChar(hsv.h, 0, 255, 0, 360, outBuff);
    publish(mqttGetHueTopic, outBuff);

    intRangeToChar(hsv.s, 0, 255, 0, 100, outBuff);
    publish(mqttGetSaturationTopic, outBuff);
}


void MqttPubSub::publishRandom(const bool isRandomMode) {
    publish(mqttGetRandomTopic, isRandomMode ? "true" : "false");
}

void MqttPubSub::setSubscriptions() {

    subscribe(mqttSetPowerTopic);
    subscribe(mqttSetHueTopic);
    subscribe(mqttSetBrightnessTopic);
    subscribe(mqttSetSaturationTopic);
    subscribe(mqttSetRandomTopic);
}


void MqttPubSub::intRangeToChar(uint16_t in, uint16_t il, uint16_t ih, uint16_t ol, uint16_t oh,char* outBuff) {
    itoa(map(in, il, ih, ol, oh), outBuff, 10);
}

MqttPubSub* MqttPubSub::setLightSaturationCallback(MqttPubSub::IntValueCallback lightSaturationCallback) {
    MqttPubSub::lightSaturationCallback = lightSaturationCallback;
    return this;
}

MqttPubSub*  MqttPubSub::setLightBrightnessCallback(MqttPubSub::IntValueCallback lightBrightnessCallback) {
    MqttPubSub::lightBrightnessCallback = lightBrightnessCallback;
    return this;
}

MqttPubSub*  MqttPubSub::setLightHueCallback(MqttPubSub::IntValueCallback lightHueCallback) {
    MqttPubSub::lightHueCallback = lightHueCallback;
    return this;
}

MqttPubSub*  MqttPubSub::setLightPowerCallback(MqttPubSub::PowerCallback lightPowerCallback) {
    MqttPubSub::lightPowerCallback = lightPowerCallback;
    return this;
}


MqttPubSub *MqttPubSub::setLightRandomCallback(const MqttPubSub::PowerCallback lightRandomCallback) {
    MqttPubSub::lightRandomCallback = lightRandomCallback;
    return this;
}

void MqttPubSub::setPayloadToIntCb(const MqttPubSub::IntValueCallback cb, byte *payload, unsigned int length, int ol, int oh) {
    char message_buff[100] = {0};
    int i = 0;
    // create character buffer with ending null terminator (string)
    for(i=0; i<length; i++) {
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    if(cb != NULL) {
        uint32_t intMsg = map(atoi(message_buff), ol, oh, 0, 255);
        syslog.logf(LOG_DEBUG, "pv: '%i'", intMsg);

        cb(intMsg);
    }
}
