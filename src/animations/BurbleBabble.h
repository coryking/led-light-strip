//
// Created by Cory King on 12/26/17.
//

#ifndef RGBWPLAY_BURBLEBABBLE_H
#define RGBWPLAY_BURBLEBABBLE_H

#include <FastLED.h>
#include <vector>
#include "Noise.h"

#define DEFAULT_DECAY_TIME 0
#define DEFAULT_DECAY_AMOUNT 10

enum GlitterType {
    GLITTER_BLACK,
    GLITTER_WHITE,
    GLITTER_COMPLIMENTARY // 180 degree
};

class BurbleBabble : public Noise {
public:
    typedef struct {
        uint8_t minHue;
        uint8_t maxHue;
        uint8_t decayAmount;
        uint16_t decayTime;
        GlitterType glitterType;
        uint16_t pieceMinTime;
        uint16_t pieceMaxTime;
        uint8_t stickFactor;
    } ConfettiHue;

    explicit BurbleBabble(uint16 numLeds);
    explicit BurbleBabble(uint16_t numLeds, uint8_t index);

    void beginAnimation() override;

    void endAnimation() override;

    virtual ~BurbleBabble();

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    void newVariant() override;

    const uint8_t getPieceDecayAmount() const;
    const uint64_t getPieceDecayTime() const;
    const GlitterType getGlitterType() const;
    const uint16_t getPieceMinTime() const;
    const uint16_t getPieceMaxTime() const;
    const uint8_t getStickFactor() const;


protected:
    void newTargetPalette() override;
    uint8_t getHue(bool invert=false);
private:
    uint8_t lowHue = HUE_YELLOW;
    uint8_t highHue = HUE_GREEN;


    CRGB *confetti= nullptr;
    uint8_t *confettiOpacity = nullptr;
    uint8_t *stickFactor = nullptr;

    uint8_t currentHues = 0;
    std::vector<ConfettiHue> confettiHues = {
            {HUE_GREEN - 10, HUE_GREEN + 10, 2, 10, GLITTER_WHITE, 10, 200,200},
            {HUE_RED - 5, HUE_RED + 5, 2, 10, GLITTER_WHITE, 10, 200, 200},
            {HUE_PURPLE - 10, HUE_PURPLE + 10, 2, 10, GLITTER_BLACK, 10, 200,200},
            {HUE_BLUE - 10, HUE_BLUE + 10, 2, 10, GLITTER_BLACK, 10, 200,200},
            //{HUE_ORANGE - 5, HUE_ORANGE + 5, 2, 10, GLITTER_BLACK, 10, 200, 200},
            //{HUE_YELLOW, HUE_GREEN, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0,50},
           /* {HUE_PURPLE, HUE_PINK, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0,10},
            {HUE_BLUE, HUE_PURPLE, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0,10},
            {HUE_RED, HUE_ORANGE, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0, 10},
            {HUE_GREEN, HUE_AQUA, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0,10},
            {HUE_GREEN, HUE_AQUA, DEFAULT_DECAY_AMOUNT, DEFAULT_DECAY_TIME, false,0,0,10},*/

    };
    uint64_t nextConfettiPieceTime = 0;
    uint64_t nextDecayTime = 0;

    void decayConfetti(CRGB *buffer) const;

    void doNewConfettiPiece();
};


#endif //RGBWPLAY_BURBLEBABBLE_H
