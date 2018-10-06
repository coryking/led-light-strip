//
// Created by Cory King on 12/10/17.
//

#ifndef RGBWPLAY_FANCYLIGHTMIXER_H
#define RGBWPLAY_FANCYLIGHTMIXER_H

#include <FastLED.h>
#include "animations/FancyLight.h"
#include "animations/AbstractPattern.h"
#include "BrightnessControl.h"
#include <map>

 struct FancyLightColor {
    CHSV color;
    uint8_t variance;
};

enum FancyLightPreset
{
    PS_NIGHT_MODE,
    PS_RED_MODE,
    PS_FULL_BRIGHT,
    PS_WARM_WHITE,
    PS_SOFT_WHITE,
    PS_COOL_WHITE,
    PS_MINT_GREEN,
    PS_SKY_BLUE,
    PS_YELLOW,
    PS_PURPLE
};


class FancyLightMixer : public AbstractPattern {
public:
    FancyLightMixer(uint16 numLeds, BrightnessControl* brightnessControl1);

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    void changePreset(FancyLightPreset newPreset);

    uint8_t getHue();
    void setHue(uint8_t hue);
    uint8_t getSaturation();
    void setSaturation(uint8_t saturation);
private:
    BrightnessControl* brightnessControl;
    FancyLight* fancyLight;

    const std::map<FancyLightPreset, FancyLightColor> lightPresets = {
            {PS_NIGHT_MODE, {.color={17, 255, 38}, .variance=6}},
            {PS_RED_MODE, {.color={0, 255, 255}, .variance=2}},
            {PS_FULL_BRIGHT, {.color={0,0,255},.variance=0}},
            {PS_WARM_WHITE, {.color={21,191,255},.variance=6}},
            {PS_SOFT_WHITE, {.color={42,79,255},.variance=6}},
            {PS_COOL_WHITE, {.color={148,76,255},.variance=6}},
            {PS_MINT_GREEN, {.color={73,224,255},.variance=6}},
            {PS_SKY_BLUE, {.color={HSVHue::HUE_AQUA, 128,255},.variance=6}},
            {PS_YELLOW, {.color={HSVHue::HUE_YELLOW,255,255},.variance=6}},
            {PS_PURPLE, {.color={HSVHue::HUE_PURPLE, 255,255},.variance=6}}
    };
};


#endif //RGBWPLAY_FANCYLIGHTMIXER_H
