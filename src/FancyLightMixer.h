//
// Created by Cory King on 12/10/17.
//

#ifndef RGBWPLAY_FANCYLIGHTMIXER_H
#define RGBWPLAY_FANCYLIGHTMIXER_H

#include "animations/FancyLight.h"
#include "animations/AbstractPattern.h"
#include "BrightnessControl.h"
#include <map>
typedef struct {
    uint8_t hue;
    uint8_t saturation;
    uint8_t brightness;
} FancyLightPresetItem;

enum FancyLightPreset
{
    PS_NIGHT_MODE,
    PS_RED_MODE,
    PS_FULL_BRIGHT,
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

    const std::map<FancyLightPreset, FancyLightPresetItem> lightPresets = {
            {PS_NIGHT_MODE, {17, 255, 38}},
            {PS_RED_MODE, {0, 255, 38}},
            {PS_FULL_BRIGHT, {255,255,255}},

    };
};


#endif //RGBWPLAY_FANCYLIGHTMIXER_H
