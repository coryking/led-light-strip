#include "devices.h"

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_DEBUG_COUNT_FRAME_RETRIES

//#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1
//#define DEBUG_ESP_OTA
//#define DEBUG_ESP_PORT Serial
#define OTA_DEBUG Serial
#define DEBUGV Serial.println
#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Task.h>
#include <WiFiUdp.h>
#include <Syslog.h>

#include "Wifi.h"
#include "config.h"
#include "animations/FancyLight.h"
#include "MqttPubSub.h"
#include "Mixer.h"
#include "RandomPatternList.h"
#include "Player.h"
#include "BrightnessControl.h"
#include "RemoteControl.h"


// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN

#define DEFAULT_BRIGHTNESS 127

#define FRAMES_PER_SECOND  60


// Define the array of leds
//CRGB leds[NUM_LEDS];

CHSV ledColorValue = CHSV(0,0,HSV_BRIGHTNESS);

MqttPubSub mqttPubSub(MQTT_SERVER, MQTT_PORT);

WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_PROTO_BSD);

TaskManager taskManager;

ulong numLoops = 0;

FancyLight fancyLightPattern(NUM_LEDS);
Player player(NUM_LEDS);
BrightnessControl brightness(MsToTaskTime(30));

#ifdef HAS_IR_REMOTE
RemoteControl remote;
#endif

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
extern uint32_t _frame_cnt;
extern uint32_t _retry_cnt;
#endif

char hostString[16] = {0};

long lastWifiReconnectAttempt = 0;

void readFromEEPROM();
void writeToEEPROM();

void showStats(uint32_t deltaTime);
FunctionTask taskShowStats(showStats, MsToTaskTime(10000));

void showRetries(uint32_t deltaTime);
FunctionTask taskShowRetries(showRetries, MsToTaskTime(30000));

void onMonitorWifi(uint32_t deltaTime);
FunctionTask taskMonitorWifi(onMonitorWifi, MsToTaskTime(1000));

void onHandleOTA(uint32_t deltaTime);

FunctionTask taskHandleOTA(onHandleOTA, MsToTaskTime(11));

void showNewColor() {
    Serial.printf("h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
    fancyLightPattern.setHue(ledColorValue.h);
    fancyLightPattern.setSaturation(ledColorValue.s);
    fancyLightPattern.changePalette();
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
    brightness.setBrightness(v);
    showNewColor();
}

void setPowerCb(bool power) {
    player.setPower(power);
}

void setRandomCb(bool randomState) {
    if(randomState) {
        player.setRandomMode();
    } else {
        player.setFixedPatternMode(&fancyLightPattern);
    }

    showNewColor();
}


void didConnectMQTT() {
    mqttPubSub.publishBrightness(brightness.getBrightness());
    mqttPubSub.publishPower(player.getPower());
    mqttPubSub.publishHSV(ledColorValue);
    mqttPubSub.publishRandom(player.getMode() == PlayerMode::Mode_RandomPattern ? true : false);
}

void remoteCallback(RemoteButtons buttons) {
    syslog.logf(LOG_INFO, "Button [%i]\n", buttons);
    Serial.printf("Button [%i]\n", buttons);
    switch(buttons) {
        case BUTTON_OK:
            player.setPower(!player.getPower());
            break;
        case BUTTON_LEFT:
            player.setRandomMode();
            break;
        case BUTTON_RIGHT:
            player.setFixedPatternMode(&fancyLightPattern);
            break;
        case BUTTON_UP:
            brightness.incrementBrightness();
            break;
        case BUTTON_DOWN:
            brightness.decrementBrightness();
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
            if(player.getMode() == Mode_FixedPattern) {
                player.setPattern((uint8_t)buttons);
            }
            break;

        default:
            break;
    }
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

    FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(player.getFastLEDBuffer(), player.getNumLeds());
    FastLED.setCorrection( TypicalLEDStrip );

    if(EEPROM.read(0) != 255) {
        syslog.log(LOG_INFO, "Nothing in EEPROM");
        brightness.setBrightness(DEFAULT_BRIGHTNESS);
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
        ->setLightRandomCallback(setRandomCb)
        ->setLightPowerCallback(setPowerCb);

    mqttPubSub.setReconnectCallback(didConnectMQTT);
    Serial.println("Gonna connect");
    if (!MDNS.begin(hostString)) {
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("Done with setup!");

#ifdef FASTLED_DEBUG_COUNT_FRAME_RETRIES
    taskManager.StartTask(&taskShowRetries);
#endif
    taskManager.StartTask(&taskShowStats);
    taskManager.StartTask(&taskMonitorWifi);
    taskManager.StartTask(&mqttPubSub);
    taskManager.StartTask(&taskHandleOTA);
    taskManager.StartTask(&player);
    taskManager.StartTask(&brightness);

#ifdef HAS_IR_REMOTE
    remote.initRemote(IR_REMOTE_PIN);
    remote.setRemoteButtonPressedEvent(remoteCallback);
    taskManager.StartTask(&remote);
#endif

}

void readFromEEPROM() {
    brightness.setBrightness(EEPROM.read(1));
    ledColorValue = EEPROM.get(2, ledColorValue);
    auto mode = (PlayerMode)EEPROM.read(3);
    if(mode != player.getMode()) {
        if(mode == PlayerMode::Mode_FixedPattern) {
            player.setFixedPatternMode(&fancyLightPattern);
        } else {
            player.setRandomMode();
        }
    }
    syslog.logf(LOG_INFO, "from eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, FastLED.getBrightness());
}

void writeToEEPROM() {
    syslog.logf(LOG_INFO, "to eeprom - h: %i, s: %i, v: %i, b: %i\n", ledColorValue.h, ledColorValue.s, ledColorValue.v, brightness.getBrightness());
    EEPROM.write(0,255);
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
