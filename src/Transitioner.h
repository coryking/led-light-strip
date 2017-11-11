//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_TRANSITIONER_H
#define RGBWPLAY_TRANSITIONER_H

#include <FastLED.h>
#define min(a,b) ((a)<(b)?(a):(b))



class Transitioner {
public:
    Transitioner() {}

    void resetTransitioner(uint32_t currentTime, uint32_t durationMs, uint8_t fromValue, uint8_t toValue) {
        this->startTime = currentTime;
        this->endTime = this->startTime + durationMs;
        this->from = fromValue;
        this->to = toValue;
    }

    virtual uint8_t getCurrentValue(uint32_t currentTime) const = 0;

    bool isActive(uint32_t currentTime) const {
        return !this->isComplete(currentTime);
    }
    bool isComplete(uint32_t currentTime) const {
        return (currentTime > endTime);
    }
protected:
    uint32_t startTime;
    uint32_t endTime;
    uint8_t from;
    uint8_t to;
};

class CubicTransitioner : public Transitioner {
public:
    uint8_t getCurrentValue(uint32_t currentTime) const override {
        uint8_t linearValue =
                (from > to) ?
                    map(currentTime, startTime, endTime, 255, 0) :
                    map(currentTime, startTime, endTime, 0, 255);


        uint8_t cubicValue = ease8InOutCubic(linearValue);
        return scale8(cubicValue, abs(from - to)) + min(from, to);
    }

    CubicTransitioner() : Transitioner() {}
};

#endif //RGBWPLAY_TRANSITIONER_H
