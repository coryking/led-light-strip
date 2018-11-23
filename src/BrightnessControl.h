//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_BRIGHTNESSCONTROL_H
#define RGBWPLAY_BRIGHTNESSCONTROL_H

#include <Task.h>
#include "Transitioner.h"

#define BRIGHTNESS_TRANSITION 1000
#define BRIGHTNESS_INCREMENT 10

class BrightnessControl : public Task {
public:
    explicit BrightnessControl(uint32_t timeInterval) : Task(timeInterval) {
        this->oldBrightness = this->brightness = FastLED.getBrightness();
    }

    void setBrightness(uint8_t brightness, bool noDelay = false) {
        uint64_t t = millis();
        //syslog.logf(LOG_INFO, "b to %d (%d)", brightness, FastLED.getBrightness());
        Serial.printf("b to %d (%d)\n", brightness, FastLED.getBrightness());
        this->oldBrightness = FastLED.getBrightness();
        this->brightness = brightness;

        uint32_t transitionSpeed = noDelay ? 0 : BRIGHTNESS_TRANSITION;

        // If no delay, lets be literal and not even wait for
        // the next time the task manager calls us....
        if (noDelay) {
            setFastLEDBrightness(brightness);
        }

        transitioner.resetTransitioner(t, noDelay, oldBrightness, brightness);
    }

    uint8_t ICACHE_FLASH_ATTR getBrightness() const {
        return this->brightness;
    }

    void ICACHE_FLASH_ATTR incrementBrightness() {
        this->setBrightness(qadd8(this->brightness, BRIGHTNESS_INCREMENT), true);
    }

    void ICACHE_FLASH_ATTR decrementBrightness() {
        this->setBrightness(qsub8(this->brightness, BRIGHTNESS_INCREMENT), true);
    }

protected:
    void OnUpdate(uint32_t deltaTime) override {
        Task::OnUpdate(deltaTime);
        uint32_t time = millis();
        if (transitioner.isActive(time)) {
            auto bright = transitioner.getCurrentValue(time);
            setFastLEDBrightness(bright);
        } else {
            setFastLEDBrightness(brightness);
        }
    }

private:
    uint8_t oldBrightness;
    uint8_t brightness;
    CubicTransitioner transitioner;

    uint8_t ICACHE_FLASH_ATTR getFastLEDBrightness() {
        return brighten8_raw(FastLED.getBrightness());
    }

    void ICACHE_FLASH_ATTR setFastLEDBrightness(uint8_t brightness) {
        FastLED.setBrightness(dim8_raw(brightness));
    }
};

#endif //RGBWPLAY_BRIGHTNESSCONTROL_H
