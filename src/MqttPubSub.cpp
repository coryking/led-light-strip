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

void MqttPubSub::publish(String &topic, const char* value) {
    if(this->mqttState == Connected) {
        syslog.logf(LOG_INFO, "p: [%s] '%s'\n", value, topic.c_str());
        this->client->publish(topic.c_str(), value);
        yield();
        this->client->loop();
        yield();
    } else {
        syslog.logf(LOG_INFO, "ig: [%s] '%s'\n", value, topic.c_str());
    }

}

void MqttPubSub::subscribe(String &topic) {
    if(this->mqttState == Connected) {
        syslog.logf(LOG_INFO, "sub '%s'\n", topic.c_str());
        this->client->subscribe(topic.c_str());
    } else {
        syslog.logf(LOG_INFO, "ignore '%s'\n", topic.c_str());

    }
}

void MqttPubSub::setReconnectCallback(const MqttPubSub::ReconnectCallback reconnectCallback) {
    MqttPubSub::reconnectCallback = reconnectCallback;
}

void MqttPubSub::getRandomClientId() {
    sprintf(espTopicId, "ESP_%06X", ESP.getChipId());
    sprintf(espClientId, "%s_%04X", espTopicId, random(0,65535));
    Serial.printf("CID:[%s]",espClientId);
    syslog.logf(LOG_INFO, "CID:[%s]",espClientId);
}

void MqttPubSub::makePubSubClient() {
    if(client != NULL)
        delete client;

    client = new PubSubClient();
    client->setClient(wifiClient);
    client->setServer(MQTT_SERVER, 1883);
    client->setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->mqttCallback(topic,payload,length);
    });


}

void MqttPubSub::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);

    if(this->client == NULL || this->mqttState == Disconnecting)
    {
        this->wifiClient.stop();
        syslog.logf(LOG_WARNING, "To Mqtt Disconnected");
        this->mqttState = Disconnected;
    }

    if(this->mqttState == Disconnected) {
        this->getRandomClientId();
        this->makePubSubClient();
        toWaiting();
    }

    if(this->mqttState == Waiting) {
        if(millis() > this->timeWaitStarted + MQTT_CONNECT_WAIT)
        {
            syslog.logf(LOG_WARNING, "To Mqtt Connecting...");
            this->mqttState = Connecting;

        }
    }

    if(this->mqttState == Connecting) {
        if(this->client->connect(espClientId)) {
            syslog.logf(LOG_WARNING, "To Mqtt Connected...");
            this->mqttState = Connected;

            // ... and resubscribe
            this->setSubscriptions();
            if(reconnectCallback != NULL)
                reconnectCallback();
        } else {
            this->toWaiting();
        }
    }

    if(this->mqttState == Connected) {
        if(!this->client->loop())
        {
            this->mqttState = Disconnecting;
        }
    }

}

void MqttPubSub::toWaiting() {
    syslog.logf(LOG_WARNING, "To Mqtt Waiting...");
    timeWaitStarted = millis();
    mqttState = Waiting;
}
