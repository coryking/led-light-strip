//
// Created by Cory King on 12/26/17.
//

#ifndef RGBWPLAY_BURBLEBABBLE_H
#define RGBWPLAY_BURBLEBABBLE_H

#include <FastLED.h>
#include <vector>
#include "Noise.h"



class BurbleBabble : public Noise {
public:
    typedef struct {
        uint8_t minHue;
        uint8_t maxHue;
    } ConfettiHue;

    explicit BurbleBabble(uint16 numLeds);

    void beginAnimation() override;

    void endAnimation() override;

    virtual ~BurbleBabble();

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    void newVariant() override;

protected:
    void newTargetPalette() override;
    uint8_t getHue(bool invert=false);
private:
    uint8_t lowHue = HUE_YELLOW;
    uint8_t highHue = HUE_GREEN;

    const uint8_t pieceMinTime=0;
    const uint8_t pieceMaxTime=0;
    const uint8_t pieceDecayAmount=10;

    CRGB *confetti= nullptr;
    uint8_t *confettiOpacity = nullptr;

    uint8_t currentHues = 0;
    std::vector<ConfettiHue> confettiHues = {
            {HUE_YELLOW, HUE_GREEN},
            {HUE_PURPLE, HUE_PINK},
            {HUE_BLUE, HUE_PURPLE},
            {HUE_RED, HUE_ORANGE},
            {HUE_GREEN, HUE_AQUA},
            {HUE_GREEN, HUE_AQUA},

    };
    uint64_t nextConfettiPieceTime = 0;

};


#endif //RGBWPLAY_BURBLEBABBLE_H
