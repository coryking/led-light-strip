#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_DEBUG_COUNT_FRAME_RETRIES
#define FASTLED_RGBW
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 1
//#define DEBUG_ESP_OTA
//#define DEBUG_ESP_PORT Serial
#define OTA_DEBUG Serial
#define DEBUGV Serial.println
#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include "Wifi.h"
#include "MqttPubSub.h"
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Task.h>
#include <WiFiUdp.h>
#include <Syslog.h>
#include "config.h"



// How many leds in your strip?
#define NUM_LEDS 180

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 4

#define DEFAULT_BRIGHTNESS 127
#define HSV_BRIGHTNESS 255

#define FRAMES_PER_SECOND  60


// Define the array of leds
CRGB leds[NUM_LEDS];

CHSV ledColorValue = CHSV(0,0,HSV_BRIGHTNESS);

PowerState powerState = POWERING_OFF;
uint8_t  lastBrightness = 0;

WiFiClient espClient;
MqttPubSub mqttPubSub(espClient);
CRGBPalette16 targetPalette = CRGBPalette16(ledColorValue);

WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_PROTO_BSD);

TaskManager taskManager;

ulong numLoops = 0;
CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
extern uint32_t _frame_cnt;
extern uint32_t _retry_cnt;
#endif

char hostString[16] = {0};

long lastWifiReconnectAttempt = 0;

void readFromEEPROM();
void writeToEEPROM();

void newTargetPalette(uint32_t deltaTime);
FunctionTask taskNewTargetPalette(newTargetPalette, MsToTaskTime(5000));

void managePower(uint32_t deltaTime);
FunctionTask taskManagePower(managePower, MsToTaskTime(1000 / FRAMES_PER_SECOND));

void showStats(uint32_t deltaTime);
FunctionTask taskShowStats(showStats, MsToTaskTime(10000));

void showRetries(uint32_t deltaTime);
FunctionTask taskShowRetries(showRetries, MsToTaskTime(30000));

void onMonitorWifi(uint32_t deltaTime);
FunctionTask taskMonitorWifi(onMonitorWifi, MsToTaskTime(1000));

void updateNoise(uint32_t deltaTime);

void onHandleMqtt(uint32_t deltaTime);
FunctionTask taskHandleMqtt(onHandleMqtt, MsToTaskTime(10));

void onHandleOTA(uint32_t deltaTime);
FunctionTask taskHandleOTA(onHandleOTA, MsToTaskTime(11));

void showNewColor() {
    Serial.printf("h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
    newTargetPalette(0);
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
    syslog.server(SYSLOG_SERVER, SYSLOG_PORT);
    syslog.deviceHostname(hostString);
    syslog.appName(APP_NAME);
    syslog.defaultPriority(LOG_DAEMON);
    syslog.logf(LOG_WARNING, "Hello from [%s]\n", hostString);
    Serial.printf("Hello from [%s]\n", hostString);
    ArduinoOTA.begin();

    FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setCorrection( TypicalLEDStrip );

    if(EEPROM.read(0) != 255) {
        syslog.log(LOG_INFO, "Nothing in EEPROM");
        lastBrightness = DEFAULT_BRIGHTNESS;
        ledColorValue = CHSV(0,0,HSV_BRIGHTNESS);
        writeToEEPROM();
    } else {
        syslog.log(LOG_INFO, "Reading from EEPROM");
        readFromEEPROM();
    }
    FastLED.setBrightness(0);
    FastLED.showColor(CRGB::Black);

    mqttPubSub
        .setLightHueCallback(setHueCb)
        ->setLightBrightnessCallback(setBrightnessCb)
        ->setLightSaturationCallback(setSaturationCb)
        ->setLightPowerCallback(setPowerCb);

    mqttPubSub.setReconnectCallback(didConnectMQTT);
    Serial.println("Gonna connect");
    mqttPubSub.connect();
    if (!MDNS.begin(hostString)) {
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("Done with setup!");

    taskManager.StartTask(&taskManagePower);
    taskManager.StartTask(&taskNewTargetPalette);
#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
    taskManager.StartTask(&taskShowRetries);
#endif
    taskManager.StartTask(&taskShowStats);
    taskManager.StartTask(&taskMonitorWifi);
    taskManager.StartTask(&taskHandleMqtt);
    taskManager.StartTask(&taskHandleOTA);

}

void readFromEEPROM() {
    lastBrightness = EEPROM.read(1);
    ledColorValue = EEPROM.get(2, ledColorValue);
    syslog.logf(LOG_INFO, "from eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
}

void writeToEEPROM() {
    syslog.logf(LOG_INFO, "to eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, lastBrightness);
    EEPROM.write(0,255);
    EEPROM.write(1, lastBrightness);
    EEPROM.put(2, ledColorValue);
    EEPROM.commit();
}

void loop() {
    ++numLoops;
    taskManager.Loop();
    yield();
}

void onHandleMqtt(uint32_t deltaTime) { mqttPubSub.loop(); }
void onHandleOTA(uint32_t deltaTime) {
    ArduinoOTA.handle();
}
void onMonitorWifi(uint32_t deltaTime) {
    if(WiFi.status() != WL_CONNECTED) {

        Serial.println("Attempting to reconnect to Wifi");
        // Attempt to reconnect
        reconnectWiFi();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Reconnected to Wifi");
            syslog.log(LOG_WARNING, "Reconnected to wifi....");
            ArduinoOTA.begin();
        }
    }
}

void updateNoise(uint32_t deltaTime) {
    static uint16_t  dist;
    static uint16_t  scale=30;
    static uint8_t maxChanges = 48;
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
    for(int i = 0; i < NUM_LEDS; i++) {                                      // Just ONE loop to fill up the LED array as all of the pixels change.
            uint8_t index = inoise8(i*scale, dist+i*scale) % 255;                  // Get a value from the noise function. I'm using both x and y axis.
            leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
        }
    dist += beatsin8(10,1, 4);                                               // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
    // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
}

void showRetries(uint32_t deltaTime) { syslog.logf(LOG_DEBUG, "R: %i, F: %i\n", _retry_cnt, _frame_cnt); }

void showStats(uint32_t deltaTime) {

    static ulong lastLoopCnt = 0;
    static char lb[100] = {0};
    syslog.logf(LOG_DEBUG, "F: %i\n", FastLED.getFPS());

    ulong dur = deltaTime / 1000;
    ulong lps = numLoops - lastLoopCnt;
    lastLoopCnt = numLoops;
    sprintf(lb, "L:%i NL:%i LPS:%u DUR:%u", lps, numLoops,lps/dur,dur);
    syslog.log(LOG_DEBUG, lb);
    Serial.println(lb);
}

void managePower(uint32_t deltaTime) {
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
            FastLED.showColor(CHSV(0, 0, 0));
            powerState = POWERED_OFF;
            mqttPubSub.publishPower(false);
        }
    if(powerState == POWER_ON) {
            updateNoise(deltaTime);
            FastLED.show();
            //FastLED.showColor(ledColorValue);
        }
}

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
void newTargetPalette(uint32_t deltaTime) {
    uint8_t ms = 0;

    if(ledColorValue.s < 127) {
        ms = min(255, ledColorValue.s + 16);
    } else {
        ms = max(0, ledColorValue.s - 64);
    }

    targetPalette = CRGBPalette16(
                CHSV(random(ledColorValue.h-6, ledColorValue.h+6), ledColorValue.s, HSV_BRIGHTNESS),// random8(128,255)),
                CHSV(random(ledColorValue.h-6, ledColorValue.h+6), ledColorValue.s, HSV_BRIGHTNESS), //random8(128,255)),
                CHSV(ledColorValue.h, ms, HSV_BRIGHTNESS), //random8(128,255)),
                CHSV(ledColorValue.h, ledColorValue.s, HSV_BRIGHTNESS)); //random8(128,255)));
}