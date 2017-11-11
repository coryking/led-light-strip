//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_PLAYER_H
#define RGBWPLAY_PLAYER_H

#include <FastLED.h>
#include <Task.h>
#include "RandomPatternList.h"
#include "Mixer.h"

#define FRAMES_PER_SECOND  60

enum PlayerMode {
    Mode_FixedPattern = 0,
    Mode_RandomPattern,
};

class Player : public Task {
public:
    Player(uint32_t numLeds) : Player(numLeds, FRAMES_PER_SECOND) {}
    Player(uint32_t numLeds, uint8_t framesPerSecond);

    void setCurrentPattern(AbstractPattern *pattern);

    CRGB *getBuffer() const;

    uint32_t getNumLeds() const;

    PlayerMode getMode() const;
    void setMode(PlayerMode mode);

    virtual ~Player();

protected:
    void OnUpdate(uint32_t deltaTime) override;

private:
    CRGB* buffer = NULL;
    uint32_t numLeds;
    PlayerMode mode = Mode_FixedPattern;

    uint32_t timeForTransition;

    Mixer* mixer;
    RandomPatternList* list;
};

#endif //RGBWPLAY_PLAYER_H
