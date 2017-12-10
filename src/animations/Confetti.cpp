//
// Created by Cory King on 1/24/17.
//

#include "Confetti.h"

uint16_t Confetti::readFrame(CRGB *buffer, ulong time) {
    HuePattern::readFrame(buffer, time);

    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( buffer, getNumLeds(), 10);
    if(nextConfettiPeiceTime <= time) {
        int pos = random16(getNumLeds());
        buffer[pos] += CHSV(gHue + random8(64), 200, 255);
        nextConfettiPeiceTime = time + confettiSpeeds[confettiSpeed];
    }
    return getNumLeds();
}

Confetti::Confetti(uint16 numLeds) : HuePattern(numLeds) {
}

void Confetti::newVariant() {
    HuePattern::newVariant();
    confettiSpeed++;
    if(confettiSpeed==confettiSpeeds.size())
        confettiSpeed=0;
    nextConfettiPeiceTime=0;
}
