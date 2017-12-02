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
    BrightnessControl(uint32_t timeInterval) : Task(timeInterval) {

    }

    void setBrightness(uint8_t brightness) {
        uint64_t t = millis();
        syslog.logf(LOG_INFO, "b to %d (%d)", brightness, FastLED.getBrightness());
        this->oldBrightness = FastLED.getBrightness();
        this->brightness = brightness;

        transitioner.resetTransitioner(t, BRIGHTNESS_TRANSITION, oldBrightness, brightness);
    }

    uint8_t getBrightness() const {
        return this->brightness;
    }

    void incrementBrightness() {
        this->setBrightness(qadd8(this->brightness, BRIGHTNESS_INCREMENT));
    }

    void decrementBrightness() {
        this->setBrightness(qsub8(this->brightness, BRIGHTNESS_INCREMENT));
    }

protected:
    void OnUpdate(uint32_t deltaTime) override {
        Task::OnUpdate(deltaTime);
        uint32_t time=millis();
        if(transitioner.isActive(time)) {
            auto bright = transitioner.getCurrentValue(time);
            FastLED.setBrightness(bright);
        } else {
            FastLED.setBrightness(brightness);
        }
    }

private:
    uint8_t oldBrightness;
    uint8_t brightness;
    CubicTransitioner transitioner;
};

#endif //RGBWPLAY_BRIGHTNESSCONTROL_H
