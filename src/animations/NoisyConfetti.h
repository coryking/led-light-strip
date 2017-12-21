//
// Created by Cory King on 12/20/17.
//

#ifndef RGBWPLAY_NOISYCONFETTI_H
#define RGBWPLAY_NOISYCONFETTI_H

#include <FastLED.h>
#include <vector>
#include "AbstractPattern.h"

#define MAX_NOISE_BRIGHTNESS 128

typedef struct {
    uint8_t pieceMinTime;
    uint8_t pieceMaxTime;
    uint8_t numPixels;
    uint8_t pieceDecayAmount;
} NoisyConfettiSetting;

class NoisyConfetti : public HuePattern {
public:
    uint16_t readFrame(CRGB *buffer, ulong time);

    NoisyConfetti(uint16 numLeds);

    void ICACHE_FLASH_ATTR newVariant() override;

private:
    uint8_t confettiSpeed = 0;
    std::vector<NoisyConfettiSetting> confettiSpeeds = {
            {0, 0, 20, 10},
            {3, 10, 10, 5},
            {15, 35, 10, 3}
    };
    uint64_t nextConfettiPieceTime = 0;
};


#endif //RGBWPLAY_NOISYCONFETTI_H
