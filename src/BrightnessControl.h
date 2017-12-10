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

    void setBrightness(uint8_t brightness, boolean noDelay=false) {
        uint64_t t = millis();
        syslog.logf(LOG_INFO, "b to %d (%d)", brightness, FastLED.getBrightness());
        this->oldBrightness = FastLED.getBrightness();
        this->brightness = brightness;

        uint32_t transitionSpeed = noDelay ? 0 : BRIGHTNESS_TRANSITION;

        // If no delay, lets be literal and not even wait for
        // the next time the task manager calls us....
        if(noDelay) {
            setFastLEDBrightness(brightness);
        }

        transitioner.resetTransitioner(t, noDelay, oldBrightness, brightness);
    }

    uint8_t getBrightness() const {
        return this->brightness;
    }

    void incrementBrightness() {
        this->setBrightness(qadd8(this->brightness, BRIGHTNESS_INCREMENT), true);
    }

    void decrementBrightness() {
        this->setBrightness(qsub8(this->brightness, BRIGHTNESS_INCREMENT), true);
    }

protected:
    void OnUpdate(uint32_t deltaTime) override {
        Task::OnUpdate(deltaTime);
        uint32_t time=millis();
        if(transitioner.isActive(time)) {
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

    uint8_t getFastLEDBrightness() {
        return brighten8_raw(FastLED.getBrightness());
    }

    void setFastLEDBrightness(uint8_t brightness) {
        FastLED.setBrightness(dim8_raw(brightness));
    }
};

#endif //RGBWPLAY_BRIGHTNESSCONTROL_H
