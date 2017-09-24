//
// Created by Cory King on 9/23/17.
//

#include "FancyLight.h"


uint16_t FancyLight::readFrame(CRGB *buffer, ulong time) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
    for(int i = 0; i < getNumLeds(); i++) {                                      // Just ONE loop to fill up the LED array as all of the pixels change.
        uint8_t index = inoise8(i*scale, dist+i*scale) % 255;                  // Get a value from the noise function. I'm using both x and y axis.
        buffer[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }
    dist += beatsin8(10,1, 4);                                               // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
    // In some sketches, I've used millis() instead of an incremented counter. Works a treat.

    EVERY_N_SECONDS_I(FANCYLIGHT_CHANGEL_PALETTE, 5) {             // Change the target palette to a random one every 5 seconds.
        changePalette();

    }
}

void FancyLight::changePalette() {
    uint8_t ms = 0;

    if(saturation < 127) {
            ms = min(255, saturation + 16);
        } else {
            ms = max(0, saturation - 64);
        }

    targetPalette = CRGBPalette16(
                CHSV(random(hue - 6, hue + 6), saturation, HSV_BRIGHTNESS),// random8(128,255)),
                CHSV(random(hue - 6, hue + 6), saturation, HSV_BRIGHTNESS), //random8(128,255)),
                CHSV(hue, ms, HSV_BRIGHTNESS), //random8(128,255)),
                CHSV(hue, saturation, HSV_BRIGHTNESS)); //random8(128,255)));
}

FancyLight::FancyLight(uint16 numLeds) : AbstractPattern(numLeds) {
    currentPalette = CRGBPalette16(CRGB::Black);
    targetPalette = OceanColors_p;
    dist = random16(12345);
}

void FancyLight::resetRuntime() {
    AbstractPattern::resetRuntime();
    minRuntime = DEFAULT_DURATION + random(0, DEFAULT_DURATION * 10);
}

uint8_t FancyLight::getHue() const {
    return hue;
}

void FancyLight::setHue(uint8_t hue) {
    FancyLight::hue = hue;
}

uint8_t FancyLight::getSaturation() const {
    return saturation;
}

void FancyLight::setSaturation(uint8_t saturation) {
    FancyLight::saturation = saturation;
}
