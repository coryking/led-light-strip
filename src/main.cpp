#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include "Wifi.h"
#include "MqttPubSub.h"


// How many leds in your strip?
#define NUM_LEDS 180

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN D2

#define DEFAULT_BRIGHTNESS 127
#define HSV_BRIGHTNESS 255

#define FRAMES_PER_SECOND  120


// Define the array of leds
CRGB leds[NUM_LEDS];

CHSV ledColorValue = CHSV(0,0,HSV_BRIGHTNESS);

PowerState powerState = POWERING_OFF;
uint8_t  lastBrightness = 0;

WiFiClient espClient;
MqttPubSub mqttPubSub(espClient);

char hostString[16] = {0};

long lastWifiReconnectAttempt = 0;

void readFromEEPROM();
void writeToEEPROM();
void showNewColor() {
    Serial.printf("h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
    writeToEEPROM();
}

void setHueCb(uint8_t h) {
    ledColorValue.h = h;
    showNewColor();
}

void setSaturationCb(uint8_t s) {
    ledColorValue.s = s;
    showNewColor();
}

void setBrightnessCb(uint8_t v) {
    FastLED.setBrightness(v);
    lastBrightness = v;
    showNewColor();
}

void setPowerCb(bool power) {
    if(power && powerState == POWERED_OFF) {
        powerState = POWERING_ON;
    }
    if (!power && powerState == POWER_ON) {
        powerState = POWERING_OFF;
    }

}

void didConnectMQTT() {
    mqttPubSub.publishBrightness(FastLED.getBrightness());
    mqttPubSub.publishPower(powerState == POWER_ON);
    mqttPubSub.publishHSV(ledColorValue);
}

void setup() {
    Serial.begin(9600);
    EEPROM.begin(sizeof(CHSV) + 2*sizeof(uint8_t));
    sprintf(hostString, "ESP_%06X", ESP.getChipId());
    Serial.println();
    Serial.println(hostString);
    setupWiFi();

    FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setCorrection( TypicalLEDStrip );

    if(EEPROM.read(0) != 255) {
        Serial.println("Nothing in EEPROM");
        lastBrightness = DEFAULT_BRIGHTNESS;
        ledColorValue = CHSV(0,0,HSV_BRIGHTNESS);
        writeToEEPROM();
    } else {
        Serial.println("Reading from EEPROM");
        readFromEEPROM();
    }
    FastLED.setBrightness(0);


    mqttPubSub
        .setLightHueCallback(setHueCb)
        ->setLightBrightnessCallback(setBrightnessCb)
        ->setLightSaturationCallback(setSaturationCb)
        ->setLightPowerCallback(setPowerCb);

    mqttPubSub.setReconnectCallback(didConnectMQTT);
    mqttPubSub.connect();

}

void readFromEEPROM() {
    lastBrightness = EEPROM.read(1);
    ledColorValue = EEPROM.get(2, ledColorValue);
    Serial.printf("from eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
}

void writeToEEPROM() {
    Serial.printf("to eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, lastBrightness);
    EEPROM.write(0,255);
    EEPROM.write(1, lastBrightness);
    EEPROM.put(2, ledColorValue);
    EEPROM.commit();
}


void loop() {
    mqttPubSub.loop();
    EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND) {
        if(powerState == POWERING_ON) {
            uint8_t brightness = lastBrightness == 0 ? DEFAULT_BRIGHTNESS : lastBrightness;
            FastLED.setBrightness(brightness);
            lastBrightness = brightness;
            powerState = POWER_ON;
            mqttPubSub.publishPower(true);
            mqttPubSub.publishHSV(ledColorValue);
            mqttPubSub.publishBrightness(brightness);
        }
        if(powerState == POWERING_OFF) {
            lastBrightness = (FastLED.getBrightness() == 0) ? lastBrightness : FastLED.getBrightness();
            FastLED.setBrightness(0);
            FastLED.showColor(CHSV(0,0,0));
            powerState = POWERED_OFF;
            mqttPubSub.publishPower(false);
        }
        if(powerState == POWER_ON) {
            FastLED.showColor(ledColorValue);
        }
    }
    EVERY_N_MILLISECONDS_I(SHOW_FPS, 5000) {
        Serial.printf("FPS: %i\n", FastLED.getFPS());
    }
    if(WiFi.status() != WL_CONNECTED) {
        long now = millis();
        if (now - lastWifiReconnectAttempt > 5000) {
            lastWifiReconnectAttempt = now;
            // Attempt to reconnect
            reconnectWiFi();

            if (WiFi.status() == WL_CONNECTED) {
                lastWifiReconnectAttempt = 0;
            }
        }
    }
}