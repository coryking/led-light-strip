//
// Created by Cory King on 12/26/17.
//

#ifndef RGBWPLAY_BURBLEBABBLE_H
#define RGBWPLAY_BURBLEBABBLE_H

#include <FastLED.h>
#include <vector>
#include "AbstractPattern.h"
#include "Noise.h"

class BurbleBabble : public Noise {
public:
    typedef struct {
        uint8_t hue;
        uint8_t opacity;
    } ConfettiNoise;


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

    ConfettiNoise *noise=NULL;

};


#endif //RGBWPLAY_BURBLEBABBLE_H
