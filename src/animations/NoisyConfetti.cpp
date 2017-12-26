//
// Created by Cory King on 12/20/17.
//

#include "NoisyConfetti.h"
#include "patterns.h"

uint16_t NoisyConfetti::readFrame(CRGB *buffer, ulong time) {
    HuePattern::readFrame(buffer,time);

    fadeToBlackBy( buffer, getNumLeds(), confettiSpeeds[confettiSpeed].pieceDecayAmount);


    for(int i=0; i < getNumLeds(); i++) {
        if(random(0, 1000) < confettiSpeeds[confettiSpeed].numPixels && buffer[i] < CRGB(127,127,127))
            buffer[i] = CHSV(random8(), random8(0, 25), random8(0, MAX_NOISE_BRIGHTNESS));
    }

    if(nextConfettiPieceTime <= time) {
        int pos = random16(getNumLeds());
        buffer[pos] += CHSV(getHue() + random8(64), 200, 255);
        nextConfettiPieceTime =
                time +
                random8(
                        confettiSpeeds[confettiSpeed].pieceMinTime,
                        confettiSpeeds[confettiSpeed].pieceMaxTime);
    }
    return getNumLeds();
}

NoisyConfetti::NoisyConfetti(uint16 numLeds) : HuePattern(numLeds) {

}

void NoisyConfetti::newVariant() {
    HuePattern::newVariant();
    confettiSpeed++;
    if(confettiSpeed==confettiSpeeds.size())
        confettiSpeed=0;
    nextConfettiPieceTime=0;
}
