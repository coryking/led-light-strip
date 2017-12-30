//
// Created by Cory King on 1/24/17.
//

#ifndef NEOPIXELLEDCONTROLLER_RAINBOW_H
#define NEOPIXELLEDCONTROLLER_RAINBOW_H

#include "AbstractPattern.h"

class Rainbow: public HuePattern {
public:
    explicit Rainbow(uint16 numLeds);

    uint16_t readFrame(CRGB *buffer, ulong time) override;
};

#endif //NEOPIXELLEDCONTROLLER_RAINBOW_H
