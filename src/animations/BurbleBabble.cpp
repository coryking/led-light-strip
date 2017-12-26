//
// Created by Cory King on 12/26/17.
//

#include "BurbleBabble.h"

BurbleBabble::BurbleBabble(uint16 numLeds) : Noise(numLeds) {

}

void BurbleBabble::beginAnimation() {
    Noise::beginAnimation();
    if(confetti==NULL)
        confetti = (CRGB*)malloc(sizeof(CRGB) * getNumLeds());

    if(confettiOpacity==NULL)
        confettiOpacity = (uint8_t*)malloc(sizeof(uint8_t) * getNumLeds());
}

void BurbleBabble::endAnimation() {
    Noise::endAnimation();
    if(confetti!=NULL)
        free(confetti);

    if(confettiOpacity!=NULL)
        free(confettiOpacity);
}

uint16_t BurbleBabble::readFrame(CRGB *buffer, ulong time) {
    Noise::readFrame(buffer, time);

    if(confetti==NULL)
        return 0;


    // random colored speckles that blink in and fade smoothly
    for(int i = 0; i < getNumLeds(); i++) {
        nblend(buffer[i], confetti[i],confettiOpacity[i]);
        confettiOpacity[i] = qsub8(confettiOpacity[i], confettiSpeeds[confettiSpeed].pieceDecayAmount);
    }
    if(nextConfettiPieceTime <= time) {
        int pos = random16(getNumLeds());
        confetti[pos] += CHSV(getHue(true), 200, random8(128, 255));
        confettiOpacity[pos] = 255;

        nextConfettiPieceTime =
                time +
                random8(
                        confettiSpeeds[confettiSpeed].pieceMinTime,
                        confettiSpeeds[confettiSpeed].pieceMaxTime);
    }
    return getNumLeds();

}

BurbleBabble::~BurbleBabble() {
    endAnimation();
}

void BurbleBabble::newTargetPalette() {
    targetPalette = CRGBPalette16(
            CHSV(getHue(), 255, random8(128,255)),
            CHSV(getHue(), 255, random8(128,255)),
            CHSV(getHue(), 192, random8(128,255)),
            CHSV(getHue(), 255, random8(128,255)));
}

uint8_t BurbleBabble::getHue(bool invert) {
    uint8_t inv = invert ? -180 : 0;
    return(random8(lowHue + inv, highHue + inv));
}
