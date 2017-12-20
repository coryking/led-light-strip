//
// Created by Cory King on 12/20/17.
//

#include "NoisyConfetti.h"

uint16_t NoisyConfetti::readFrame(CRGB *buffer, ulong time) {
    HuePattern::readFrame(buffer,time);

    for(int i=0; i < getNumLeds(); i++) {
        buffer[i] = CHSV(255, 0, random8(0, MAX_NOISE_BRIGHTNESS));
    }

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

NoisyConfetti::NoisyConfetti(uint16 numLeds) : HuePattern(numLeds) {

}

void NoisyConfetti::newVariant() {
    HuePattern::newVariant();
    confettiSpeed++;
    if(confettiSpeed==confettiSpeeds.size())
        confettiSpeed=0;
    nextConfettiPieceTime=0;
}
