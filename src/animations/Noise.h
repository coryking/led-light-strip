//
// Created by Cory King on 1/24/17.
//

#ifndef NEOPIXELLEDCONTROLLER_NOISE_H
#define NEOPIXELLEDCONTROLLER_NOISE_H

#include <FastLED.h>
#include "AbstractPattern.h"

class Noise : public AbstractPattern{
private:
    uint16_t  dist;
    uint16_t  scale=30;
    uint8_t maxChanges = 48;
    uint16_t  minRuntime = DEFAULT_DURATION;
protected:
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    virtual void newTargetPalette();

public:
    virtual uint16_t readFrame(CRGB *buffer, ulong time);

    Noise(uint16 numLeds);

    virtual void resetRuntime();

    virtual uint16_t getMinRuntime() {
        return minRuntime;
    }

    void newVariant() override;

};


#endif //NEOPIXELLEDCONTROLLER_NOISE_H
