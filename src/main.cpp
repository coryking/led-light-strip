#include "devices.h"

#define FASTLED_ESP8266_RAW_PIN_ORDER


//#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1
//#define DEBUG_ESP_OTA
//#define DEBUG_ESP_PORT Serial
#define OTA_DEBUG Serial
#define DEBUGV Serial.println

#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <Task.h>
#include <WiFiUdp.h>

#include "config.h"
#include "animations/FancyLight.h"

#include "Wifi.h"

#ifndef DO_NOT_USE_WIFI

#include "MqttPubSub.h"
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#endif

#include "Player.h"
#include "BrightnessControl.h"
#include "RemoteControl.h"
#include "FancyLightMixer.h"



// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN

#define DEFAULT_BRIGHTNESS 127

// Define the array of leds
//CRGB leds[NUM_LEDS];

CHSV ledColorValue = CHSV(0, 0, HSV_BRIGHTNESS);

#ifndef DO_NOT_USE_WIFI
MqttPubSub mqttPubSub(MQTT_SERVER, MQTT_PORT);

#endif
WiFiUDP udpClient;

Syslog syslog(udpClient, SYSLOG_PROTO_BSD);

TaskManager taskManager;

ulong numLoops = 0;

FancyLightMixer *fancyLightMixer;
Player player(NUM_LEDS);
BrightnessControl brightness(MsToTaskTime(30));

#ifdef HAS_IR_REMOTE
RemoteControl remote;
#endif

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
extern uint32_t _frame_cnt;
extern uint32_t _retry_cnt;
#endif

long lastWifiReconnectAttempt = 0;

void readFromEEPROM();

void writeToEEPROM();

#ifdef DEBUG_SHOW_STATS
void ICACHE_FLASH_ATTR showStats(uint32_t deltaTime);
FunctionTask taskShowStats(showStats, MsToTaskTime(10000));
#endif

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
void ICACHE_FLASH_ATTR showRetries(uint32_t deltaTime);
FunctionTask taskShowRetries(showRetries, MsToTaskTime(30000));
#endif

void ICACHE_FLASH_ATTR showNewColor() {
    Serial.printf("h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v,
                  FastLED.getBrightness());
    fancyLightMixer->setHue(ledColorValue.h);
    fancyLightMixer->setSaturation(ledColorValue.s);
    player.setPower(true); // always make sure the power is on when we show a new color...
    player.setFancyLightMode();
    writeToEEPROM();
}

void ICACHE_FLASH_ATTR setHueCb(uint8_t h) {
    ledColorValue.h = h;
    showNewColor();
}

void ICACHE_FLASH_ATTR setSaturationCb(uint8_t s) {
    ledColorValue.s = s;
    showNewColor();
}

void ICACHE_FLASH_ATTR setBrightnessCb(uint8_t v) {
    brightness.setBrightness(v);
    showNewColor();
}

void ICACHE_FLASH_ATTR setPowerCb(bool power) {
    player.setPower(power);
}

void ICACHE_FLASH_ATTR setRandomCb(bool randomState) {
    if (randomState) {
        player.setRandomMode();
    } else {
        player.setFixedPatternMode(fancyLightMixer);
    }

    showNewColor();
}

#ifndef DO_NOT_USE_WIFI

void onMonitorWifi(uint32_t deltaTime);

FunctionTask taskMonitorWifi(onMonitorWifi, MsToTaskTime(1000));

void onHandleOTA(uint32_t deltaTime);

FunctionTask taskHandleOTA(onHandleOTA, MsToTaskTime(11));

void didConnectMQTT() {
    mqttPubSub.publishBrightness(brightness.getBrightness());
    mqttPubSub.publishPower(player.getPower());
    mqttPubSub.publishHSV(ledColorValue);
    mqttPubSub.publishRandom(player.getMode() == PlayerMode::Mode_RandomPattern);
}

#endif

void ICACHE_FLASH_ATTR remoteCallback(RemoteButtons buttons) {
    syslog.logf(LOG_INFO, "Button [%i]\n", buttons);
    Serial.printf("Button [%i]\n", buttons);
    switch (buttons) {
        case BUTTON_OK:
            player.setPower(!player.getPower());
            break;
        case BUTTON_LEFT:
            player.setRandomMode();
            break;
        case BUTTON_RIGHT:
            player.setFancyLightMode();
            break;
        case BUTTON_UP:
            brightness.incrementBrightness();
            break;
        case BUTTON_DOWN:
            brightness.decrementBrightness();
            break;
        case BUTTON_ASTERISK:
            player.setFancyLightMode(FancyLightPreset::PS_FULL_BRIGHT);
            break;
        case BUTTON_POUND:
            player.setFancyLightMode(FancyLightPreset::PS_NIGHT_MODE);
            break;
        case BUTTON_0:
        case BUTTON_1:
        case BUTTON_2:
        case BUTTON_3:
        case BUTTON_4:
        case BUTTON_5:
        case BUTTON_6:
        case BUTTON_7:
        case BUTTON_8:
        case BUTTON_9:
            // power up no matter what....
            player.setPower(true);
            player.setPattern((uint8_t) buttons);

            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(9600);
    EEPROM.begin(sizeof(CHSV) + 2 * sizeof(uint8_t));
    //sprintf(hostString, "ESP_%06X", subName); //ESP.getChipId());
    Serial.println();
    Serial.println(hostString);
#ifndef DO_NOT_USE_WIFI
    setupWiFi(hostString);
    syslog.server(SYSLOG_SERVER, SYSLOG_PORT);
    syslog.deviceHostname(hostString);

    if (!MDNS.begin(hostString)) {
        Serial.println("Error setting up MDNS responder!");
    }

    ArduinoOTA.setHostname(hostString);
    ArduinoOTA.begin();
#endif

    syslog.appName(APP_NAME);
    syslog.defaultPriority(LOG_DAEMON);
    syslog.logf(LOG_WARNING, "Hello from [%s]\n", hostString);
    Serial.printf("Hi from [%s]\n", hostString);

#ifdef MULTI_LED
    FastLED.addLeds<SK6812_PORTA, NUM_STRIPS>(player.getFastLEDBuffer(), LEDS_PER_STRIP);
#else
    FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(player.getFastLEDBuffer(), player.getNumLeds());
#endif
    // TODO: FIX THIS!!!
    //FastLED.setDither(0);

    FastLED.setCorrection(TypicalLEDStrip);

    fancyLightMixer = new FancyLightMixer(NUM_LEDS, &brightness);
    player.setFancyLight(fancyLightMixer);

    if (EEPROM.read(0) != 255) {
        syslog.log(LOG_INFO, "Nothing in EEPROM");
        brightness.setBrightness(DEFAULT_BRIGHTNESS);
        ledColorValue = CHSV(0, 0, HSV_BRIGHTNESS);
        writeToEEPROM();
    } else {
        syslog.log(LOG_INFO, "Reading from EEPROM");
        readFromEEPROM();
    }
    FastLED.setBrightness(0);
    FastLED.showColor(CRGB::Black);

#ifndef DO_NOT_USE_WIFI
    mqttPubSub
            .setLightHueCallback(setHueCb)
            ->setLightBrightnessCallback(setBrightnessCb)
            ->setLightSaturationCallback(setSaturationCb)
            ->setLightRandomCallback(setRandomCb)
            ->setLightPowerCallback(setPowerCb);

    mqttPubSub.setReconnectCallback(didConnectMQTT);
    taskManager.StartTask(&taskMonitorWifi);
    taskManager.StartTask(&mqttPubSub);
    Serial.println("Gonna connect");

    Serial.println("Done with setup!");
    taskManager.StartTask(&taskHandleOTA);
#endif

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
    taskManager.StartTask(&taskShowRetries);
#endif
#ifdef DEBUG_SHOW_STATS
    taskManager.StartTask(&taskShowStats);
#endif
    taskManager.StartTask(&player);
    taskManager.StartTask(&brightness);

#ifdef HAS_IR_REMOTE
    remote.initRemote(IR_REMOTE_PIN);
    remote.setRemoteButtonPressedEvent(remoteCallback);
    taskManager.StartTask(&remote);
#endif

}

void ICACHE_FLASH_ATTR readFromEEPROM() {
    brightness.setBrightness(EEPROM.read(1));
    ledColorValue = EEPROM.get(2, ledColorValue);
    auto mode = (PlayerMode) EEPROM.read(3);
    if (mode != player.getMode()) {
        if (mode == PlayerMode::Mode_FixedPattern) {
            player.setFixedPatternMode(fancyLightMixer);
        } else {
            player.setRandomMode();
        }
    }
    syslog.logf(LOG_INFO, "from eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s,
                ledColorValue.v, FastLED.getBrightness());
}

void ICACHE_FLASH_ATTR writeToEEPROM() {
    syslog.logf(LOG_INFO, "to eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v,
                brightness.getBrightness());
    EEPROM.write(0, 255);
    EEPROM.write(1, brightness.getBrightness());
    EEPROM.put(2, ledColorValue);
    EEPROM.put(3, player.getMode());
    EEPROM.commit();
}

void loop() {
    ++numLoops;
    taskManager.Loop();
    yield();
}

#ifndef DO_NOT_USE_WIFI

void onHandleOTA(uint32_t deltaTime) {
    ArduinoOTA.handle();
}

void onMonitorWifi(uint32_t deltaTime) {
    if (WiFi.status() != WL_CONNECTED) {

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

#endif

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES

void ICACHE_FLASH_ATTR showRetries(uint32_t deltaTime) {
    syslog.logf(LOG_DEBUG, "R: %i, F: %i\n", _retry_cnt, _frame_cnt);
}

#endif

#ifdef DEBUG_SHOW_STATS

void ICACHE_FLASH_ATTR showStats(uint32_t deltaTime) {

    static ulong lastLoopCnt = 0;
    static char lb[100] = {0};
    syslog.logf(LOG_DEBUG, "F: %i\n", FastLED.getFPS());
    Serial.printf("F: %i\n", FastLED.getFPS());


    ulong dur = deltaTime / 1000;
    ulong lps = numLoops - lastLoopCnt;
    lastLoopCnt = numLoops;
    sprintf(lb, "L:%i NL:%i LPS:%u DUR:%u", lps, numLoops, lps / dur, dur);
    syslog.log(LOG_DEBUG, lb);
    Serial.println(lb);
}

#endif