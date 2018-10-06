//
// Created by Cory King on 12/26/17.
//

#include "BurbleBabble.h"
#include "patterns.h"

#define MAX(A, B) (( (A) > (B) ) ? (A) : (B))
#define MIN(X,Y) (((X)<(Y)) ? (X):(Y))

BurbleBabble::BurbleBabble(uint16 numLeds) : Noise(numLeds) {

}

void BurbleBabble::beginAnimation() {
    Noise::beginAnimation();
    if(confetti== nullptr)
        confetti = (CRGB*)malloc(sizeof(CRGB) * getNumLeds());

    if(confettiOpacity== nullptr)
        confettiOpacity = (uint8_t*)malloc(sizeof(uint8_t) * getNumLeds());
}

void BurbleBabble::endAnimation() {
    Noise::endAnimation();
    if(confetti!= nullptr) {
        free(confetti);
        confetti = nullptr;
    }

    if(confettiOpacity!= nullptr) {
        free(confettiOpacity);
        confettiOpacity = nullptr;
    }
}

uint16_t BurbleBabble::readFrame(CRGB *buffer, ulong time) {
    Noise::readFrame(buffer, time);

    if(confetti== nullptr || confettiOpacity== nullptr)
        return 0;


    // random colored speckles that blink in and fade smoothly
    for(int i = 0; i < getNumLeds(); i++) {
        nblend(buffer[i], confetti[i],confettiOpacity[i]);
        confettiOpacity[i] = qsub8(confettiOpacity[i], getPieceDecayAmount());
    }
    if(nextConfettiPieceTime <= time) {
        int pos = random16(getNumLeds());

        if(getUseBlackGlitter()){
            confetti[pos] = CHSV(255,255,0);
        } else {
            confetti[pos] = CHSV(getHue(true), random8(200, 255), 255);
        }
        confettiOpacity[pos] = 255;

        nextConfettiPieceTime =
                time +
                random8(
                        pieceMinTime,
                        pieceMaxTime);
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
    uint8_t inv = invert ? 180 : 0;
    uint8_t m = MIN(confettiHues[currentHues].minHue - inv, confettiHues[currentHues].maxHue - inv);
    uint8_t ma = MAX(confettiHues[currentHues].minHue - inv, confettiHues[currentHues].maxHue - inv);
    return (random8(m, ma));

}

void BurbleBabble::newVariant() {
    Noise::newVariant();
    currentHues++;
    if(currentHues==confettiHues.size())
        currentHues=0;
    nextConfettiPieceTime=0;
    newTargetPalette();
}

const uint8_t BurbleBabble::getPieceDecayAmount() const {
    return confettiHues[currentHues].decayTime;
}

const bool BurbleBabble::getUseBlackGlitter() const {
    return confettiHues[currentHues].useBlackGlitter;
}
