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


}

BurbleBabble::~BurbleBabble() {
    endAnimation();
}

void BurbleBabble::newTargetPalette() {
    targetPalette = CRGBPalette16(
            CHSV(random8(lowHue, highHue), 255, random8(128,255)),
            CHSV(random8(lowHue, highHue), 255, random8(128,255)),
            CHSV(random8(lowHue, highHue), 192, random8(128,255)),
            CHSV(random8(lowHue, highHue), 255, random8(128,255)));
}
