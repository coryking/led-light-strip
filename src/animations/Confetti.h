//
// Created by Cory King on 1/24/17.
//

#ifndef NEOPIXELLEDCONTROLLER_CONFETTI_H
#define NEOPIXELLEDCONTROLLER_CONFETTI_H

#include <FastLED.h>
#include <vector>
#include "AbstractPattern.h"


class Confetti : public HuePattern {
public:
    uint16_t readFrame(CRGB *buffer, ulong time);

    Confetti(uint16 numLeds);

    void newVariant() override;

private:
    uint8_t confettiSpeed = 0;
    std::vector confettiSpeeds = {0, 200, 600};
    uint64_t nextConfettiPeiceTime = 0;
};


#endif //NEOPIXELLEDCONTROLLER_CONFETTI_H
