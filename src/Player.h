//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_PLAYER_H
#define RGBWPLAY_PLAYER_H

#include <FastLED.h>
#include <Task.h>
#include "RandomPatternList.h"
#include "Mixer.h"
#include "Transitioner.h"
#include "animations/SolidColor.h"

#define FRAMES_PER_SECOND  60
#define DEFAULT_BRIGHTNESS 255
#define POWER_TRANSITION_MS 1000

typedef enum {
    Mode_FixedPattern = 0,
    Mode_RandomPattern
} PlayerMode;

typedef enum {
    PLAYER_POWERING_ON,
    PLAYER_POWER_ON,
    PLAYER_POWERING_OFF,
    PLAYER_POWERED_OFF,

} PlayerPowerState;

class Player : public Task {
public:
    Player(uint32_t numLeds) : Player(numLeds, FRAMES_PER_SECOND) {}
    Player(uint32_t numLeds, uint8_t framesPerSecond);

    CRGB *getFastLEDBuffer() const;

    uint32_t getNumLeds() const;

    PlayerMode getMode() const;
    void setRandomMode();
    void setFixedPatternMode(AbstractPattern* pattern);

    void setPower(bool power);
    bool getPower() const;

    virtual ~Player();

protected:
    void OnUpdate(uint32_t deltaTime) override;

private:
    CRGB* buffer = NULL;
    uint32_t numLeds;
    PlayerMode mode = Mode_FixedPattern;
    PlayerMode savedMode;
    AbstractPattern* savedPattern;

    Mixer* mixer;
    RandomPatternList* list;

    SolidColor* offColor;

    PlayerPowerState powerState = PLAYER_POWERED_OFF;
};

#endif //RGBWPLAY_PLAYER_H
