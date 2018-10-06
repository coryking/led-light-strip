//
// Created by Cory King on 9/23/17.
//

#ifndef RGBWPLAY_FANCYLIGHT_H
#define RGBWPLAY_FANCYLIGHT_H

#include <FastLED.h>
#include "AbstractPattern.h"
#define HSV_BRIGHTNESS 255
#define DEFAULT_VARIANCE 6

class FancyLight : public AbstractPattern {
private:
    uint16_t  dist;
    uint16_t  scale=30;
    uint8_t maxChanges = 48;
    CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);
    CRGBPalette16 targetPalette; //(OceanColors_p);
    uint16_t  minRuntime = DEFAULT_DURATION;
    uint8_t hue;
    uint8_t saturation;
    uint8_t variance = DEFAULT_VARIANCE;

public:
    uint8_t getHue() const;

    void setHue(uint8_t hue);

    uint8_t getSaturation() const;

    void setSaturation(uint8_t saturation);

    uint8_t getVariance() const;

    void setVariance(uint8_t variance);

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    explicit FancyLight(uint16 numLeds);

    void resetRuntime() override;

    uint16_t getMinRuntime() override {
        return minRuntime;
    }

    void changePalette();

};


#endif //RGBWPLAY_FANCYLIGHT_H
