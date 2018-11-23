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

    if(stickFactor== nullptr)
        stickFactor = (uint8_t*)malloc(sizeof(uint8_t) * getNumLeds());
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

    if(stickFactor!= nullptr) {
        free(stickFactor);
        stickFactor= nullptr;
    }
}

uint16_t BurbleBabble::readFrame(CRGB *buffer, ulong time) {
    Noise::readFrame(buffer, time);

    if(confetti== nullptr || confettiOpacity== nullptr)
        return 0;

    if(nextDecayTime <= time) {
        decayConfetti(buffer);
        nextDecayTime = time + getPieceDecayTime();
    }

    if(nextConfettiPieceTime <= time) {
        doNewConfettiPiece();

        nextConfettiPieceTime =
                time +
                random16(
                        getPieceMinTime(),
                        getPieceMaxTime());
    }
    return getNumLeds();

}

void BurbleBabble::doNewConfettiPiece() {
    int pos = random16(getNumLeds());

    switch(getGlitterType()) {
            case GLITTER_BLACK:
                confetti[pos] = CHSV(255, 0, 0);
                break;
            case GLITTER_WHITE:
                confetti[pos] = CHSV(255, 0, 255);
                break;
            case GLITTER_COMPLIMENTARY:
                confetti[pos] = CHSV(getHue(true), random8(200, 255), 255);
                break;
        }

    confettiOpacity[pos] = 255;
    stickFactor[pos] = getStickFactor();
}

void BurbleBabble::decayConfetti(CRGB *buffer) const {// random colored speckles that blink in and fade smoothly
    for (int i = 0; i < getNumLeds(); i++) {
            nblend(buffer[i], confetti[i], confettiOpacity[i]);
            if(stickFactor[i] == 0) {
                confettiOpacity[i] = qsub8(confettiOpacity[i], getPieceDecayAmount());
            } else {
                stickFactor[i] = qsub8(stickFactor[i], 1);
            }
        }
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
    //nextConfettiPieceTime=0;
    /*for (int i = 0; i < getNumLeds(); i++) {
        stickFactor[i] = 0;
        confettiOpacity[i] = 0;
        confetti[i] = CHSV(255,0,0);
    }*/
    //newTargetPalette();
}

const uint8_t BurbleBabble::getPieceDecayAmount() const {
    return confettiHues[currentHues].decayAmount;
}

const GlitterType BurbleBabble::getGlitterType() const {
    return confettiHues[currentHues].glitterType;
}

const uint16_t BurbleBabble::getPieceMinTime() const {
    return confettiHues[currentHues].pieceMinTime;
}

const uint16_t BurbleBabble::getPieceMaxTime() const {
    return confettiHues[currentHues].pieceMaxTime;
}

const uint64_t BurbleBabble::getPieceDecayTime() const {
    return confettiHues[currentHues].decayTime;
}

BurbleBabble::BurbleBabble(uint16_t numLeds, uint8_t index) : Noise(numLeds) {
    currentHues =  index;
}

const uint8_t BurbleBabble::getStickFactor() const {
    return confettiHues[currentHues].stickFactor;
}
