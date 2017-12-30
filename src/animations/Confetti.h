//
// Created by Cory King on 1/24/17.
//

#ifndef NEOPIXELLEDCONTROLLER_CONFETTI_H
#define NEOPIXELLEDCONTROLLER_CONFETTI_H

#include <FastLED.h>
#include <vector>
#include "AbstractPattern.h"

typedef struct {
    uint8_t pieceMinTime;
    uint8_t pieceMaxTime;
    uint8_t pieceDecayAmount;
} ConfettiTime;

class Confetti : public HuePattern {
public:
    uint16_t readFrame(CRGB *buffer, ulong time) override;

    explicit Confetti(uint16 numLeds);

    void newVariant() override;

private:
    uint8_t confettiSpeed = 0;
    std::vector<ConfettiTime> confettiSpeeds = {
            {0, 0, 10},
            {3, 10, 5},
            {15, 35, 3}
    };
    uint64_t nextConfettiPieceTime = 0;
};


#endif //NEOPIXELLEDCONTROLLER_CONFETTI_H
