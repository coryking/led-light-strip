//
// Created by Cory King on 1/28/17.
//

#ifndef NEOPIXELLEDCONTROLLER_BPMPATTERN_H
#define NEOPIXELLEDCONTROLLER_BPMPATTERN_H

#include <Arduino.h>
#include <FastLED.h>
#include "AbstractPattern.h"

#define INVERSE_WIDTH 10

class BpmPattern : public HuePattern {
protected:
    uint8_t  BeatsPerMinute = 62;
    CRGBPalette16 palette = (CRGBPalette16)CloudColors_p;
public:
    void newVariant() override {
        BeatsPerMinute = random8(50, 80);
    }

public:
    uint16_t readFrame(CRGB *buffer, ulong time) override {

        HuePattern::readFrame(buffer,time);
        uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);

        for( int i = 0; i < getNumLeds(); i++) { //9948
            buffer[i] = ColorFromPalette(palette, getHue()+(i*2), beat-getHue()+(i*INVERSE_WIDTH));
        }
    }

    explicit BpmPattern(uint16 numLeds) : HuePattern(numLeds) {}
};

#endif //NEOPIXELLEDCONTROLLER_BPMPATTERN_H
