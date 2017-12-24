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
    PS_ORANGE
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

    const std::map<FancyLightPreset, CHSV> lightPresets = {
            {PS_NIGHT_MODE, {17, 255, 38}},
            {PS_RED_MODE, {0, 255, 38}},
            {PS_FULL_BRIGHT, {255,0,255}},
            {PS_WARM_WHITE, {21,191,255}},
            {PS_SOFT_WHITE, {42,79,255}},
            {PS_COOL_WHITE, {148,76,255}},
            {PS_MINT_GREEN, {73,224,255}},
            {PS_SKY_BLUE, {HSVHue::HUE_AQUA, 128,255}},
            {PS_YELLOW, {HSVHue::HUE_YELLOW,255,255}},
            {PS_ORANGE, {HSVHue::HUE_ORANGE, 255,255}}
    };
};


#endif //RGBWPLAY_FANCYLIGHTMIXER_H
