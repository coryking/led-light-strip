//
// Created by Cory King on 1/24/17.
//

#include "Confetti.h"

uint16_t Confetti::readFrame(CRGB *buffer, ulong time) {
    HuePattern::readFrame(buffer, time);

    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( buffer, getNumLeds(), confettiSpeeds[confettiSpeed].pieceDecayAmount);
    if(nextConfettiPieceTime <= time) {
        int pos = random16(getNumLeds());
        buffer[pos] += CHSV(gHue + random8(64), 200, 255);
        nextConfettiPieceTime =
                time +
                random8(
                        confettiSpeeds[confettiSpeed].pieceMinTime,
                        confettiSpeeds[confettiSpeed].pieceMaxTime);
    }
    return getNumLeds();
}

Confetti::Confetti(uint16 numLeds) : HuePattern(numLeds) {
}

void ICACHE_FLASH_ATTR Confetti::newVariant() {
    HuePattern::newVariant();
    confettiSpeed++;
    if(confettiSpeed==confettiSpeeds.size())
        confettiSpeed=0;
    nextConfettiPieceTime=0;
}
