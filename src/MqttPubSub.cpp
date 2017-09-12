//
// Created by Cory King on 9/12/17.
//

#include "MqttPubSub.h"


void MqttPubSub::mqttCallback(char *topic, byte *payload, unsigned int length) {

    Serial.printf("Message arrived [%s]\n", topic);

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
    if (theTopic == mqttSetPowerTopic && lightPowerCallback != NULL) {
        if ( length > 0 && payload[0] == 't') {
            lightPowerCallback(true);
        } else {
            lightPowerCallback(false);
        }
    }
}

void MqttPubSub::publishPower(const bool isOn) {
    if (isOn) {
        publish(mqttGetPowerTopic, "true");
    } else {
        publish(mqttGetPowerTopic, "false");
    }
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


void MqttPubSub::setSubscriptions() {

    subscribe(mqttSetPowerTopic);
    subscribe(mqttSetHueTopic);
    subscribe(mqttSetBrightnessTopic);
    subscribe(mqttSetSaturationTopic);
}


void MqttPubSub::loop() {

    if (!this->client.connected()) {
        ulong now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (this->reconnect()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        // Client connected
        this->client.loop();
    }

}

bool MqttPubSub::reconnect() {
    if (client.connect(this->espHostString.c_str())) {
        // ... and resubscribe
        this->setSubscriptions();
        if(reconnectCallback != NULL)
            reconnectCallback();
    }
    return client.connected();
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
        Serial.println(intMsg);

        cb(intMsg);
    }
}

void MqttPubSub::publish(String &topic, const char* value) {
    Serial.printf("pub: [%s] to '%s'\n", value, topic.c_str());
    this->client.publish(topic.c_str(), value);

}

void MqttPubSub::subscribe(String &topic) {
    Serial.printf("Subscribing to '%s'\n", topic.c_str());
    this->client.subscribe(topic.c_str());
}

void MqttPubSub::connect() {
    Serial.print("Connecting to MQTT");
    int attempts=0;
    while(!this->client.connected() && attempts < 20) {
        Serial.print(".");
        this->reconnect();

        attempts++;
        delay(100);
    }
    Serial.println();
    Serial.println("Connected to MQTT...");
}

void MqttPubSub::setReconnectCallback(const MqttPubSub::ReconnectCallback reconnectCallback) {
    MqttPubSub::reconnectCallback = reconnectCallback;
}
