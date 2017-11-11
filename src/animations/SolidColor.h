//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_SOLIDCOLOR_H
#define RGBWPLAY_SOLIDCOLOR_H

#include "Playable.h"
#include <FastLED.h>

class SolidColor : public Playable  {
public:
    uint16_t readFrame(CRGB *buffer, ulong time) override {
        fill_solid(buffer, getNumLeds(), CRGB::Black);
        return getNumLeds();
    }

    SolidColor(uint16_t numLeds) : Playable(numLeds) {

    }
};


#endif //RGBWPLAY_SOLIDCOLOR_H
