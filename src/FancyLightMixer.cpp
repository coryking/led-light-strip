//
// Created by Cory King on 12/10/17.
//

#include "FancyLightMixer.h"

FancyLightMixer::FancyLightMixer(uint16 numLeds, BrightnessControl* brightnessControl) :
        AbstractPattern(numLeds),
        brightnessControl(brightnessControl) {

    fancyLight = new FancyLight(numLeds);

}

uint16_t FancyLightMixer::readFrame(CRGB *buffer, ulong time) {
    return this->fancyLight->readFrame(buffer,time);
}

void FancyLightMixer::changePreset(FancyLightPreset newPreset) {
    FancyLightColor item = this->lightPresets.at(newPreset);

    this->fancyLight->setSaturation(item.color.saturation);
    this->fancyLight->setHue(item.color.hue);
    this->brightnessControl->setBrightness(item.color.value);
    this->fancyLight->changePalette();
}

uint8_t FancyLightMixer::getHue() {
    return this->fancyLight->getHue();
}

uint8_t FancyLightMixer::getSaturation() {
    return this->fancyLight->getRuntime();
}

void FancyLightMixer::setHue(uint8_t hue) {
    this->fancyLight->setHue(hue);
    this->fancyLight->changePalette();
}

void FancyLightMixer::setSaturation(uint8_t saturation) {
    this->fancyLight->setSaturation(saturation);
    this->fancyLight->changePalette();
}
