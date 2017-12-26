//
// Created by Cory King on 12/26/17.
//

#include "BurbleBabble.h"

BurbleBabble::BurbleBabble(uint16 numLeds) : Noise(numLeds) {

}

void BurbleBabble::beginAnimation() {
    Noise::beginAnimation();
    if(noise==NULL)
        noise = (ConfettiNoise*)malloc(sizeof(ConfettiNoise) * getNumLeds());
}

void BurbleBabble::endAnimation() {
    Noise::endAnimation();
    if(noise!=NULL)
        free(noise);
}

uint16_t BurbleBabble::readFrame(CRGB *buffer, ulong time) {
    Noise::readFrame(buffer, time);

    if(noise==NULL)
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
