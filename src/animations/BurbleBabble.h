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
        uint8_t pieceMinTime;
        uint8_t pieceMaxTime;
        uint8_t pieceDecayAmount;
    } ConfettiNoiseTime;

    BurbleBabble(uint16 numLeds);

    void beginAnimation() override;

    void endAnimation() override;

    virtual ~BurbleBabble();

    uint16_t readFrame(CRGB *buffer, ulong time) override;

protected:
    void newTargetPalette() override;

private:
    const uint8_t lowHue = HUE_YELLOW;
    const uint8_t highHue = HUE_GREEN;

    CRGB *noise=NULL;
    uint8_t *noiseOpacity = NULL:

    uint8_t confettiSpeed = 0;
    std::vector<ConfettiNoiseTime> confettiSpeeds = {
            {0, 0, 10},
            {3, 10, 5},
            {15, 35, 3}
    };
    uint64_t nextConfettiPieceTime = 0;

};


#endif //RGBWPLAY_BURBLEBABBLE_H
