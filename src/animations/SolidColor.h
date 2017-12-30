//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_SOLIDCOLOR_H
#define RGBWPLAY_SOLIDCOLOR_H

#include "AbstractPattern.h"
#include <FastLED.h>

class SolidColor : public AbstractPattern  {
public:
    uint16_t readFrame(CRGB *buffer, ulong time) override {
        fill_solid(buffer, getNumLeds(), CRGB::Black);
        return getNumLeds();
    }

    explicit SolidColor(uint16_t numLeds) : AbstractPattern(numLeds) {

    }

    uint16_t getMinRuntime() override {
        return 0;
    }
};


#endif //RGBWPLAY_SOLIDCOLOR_H
