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
#include "FancyLightMixer.h"

#define FRAMES_PER_SECOND  200
#define DEFAULT_BRIGHTNESS 255
#define POWER_TRANSITION_MS 2000

typedef enum {
    Mode_FixedPattern = 0,
    Mode_RandomPattern,
    Mode_FancyLightPattern
} PlayerMode;

typedef enum {
    PLAYER_POWERING_ON,
    PLAYER_POWER_ON,
    PLAYER_POWERING_OFF,
    PLAYER_POWERED_OFF,

} PlayerPowerState;

class Player : public Task {
public:
    explicit Player(uint32_t numLeds) : Player(numLeds, FRAMES_PER_SECOND, nullptr) {}

    Player(uint32_t numLeds, uint8_t framesPerSecond, FancyLightMixer* fancyLightMixer);

    CRGB *getFastLEDBuffer() const;

    uint32_t getNumLeds() const;

    PlayerMode getMode() const;

    void setRandomMode();
    void setFancyLightMode();
    void setFancyLightMode(FancyLightPreset preset);
    void setPattern(uint8_t patternNumber);

    void setFixedPatternMode(AbstractPattern *pattern, uint16_t transitionTime = DEFAULT_TRANSITION_TIME);
    void setFancyLight(FancyLightMixer *fancyLight);
    void setPower(bool power);

    bool getPower() const;

    virtual ~Player();

protected:
    void OnUpdate(uint32_t deltaTime) override;

private:
    FancyLightMixer* fancyLight;
    CRGB *buffer = nullptr;
    uint32_t numLeds;
    PlayerMode mode = Mode_FixedPattern;
    PlayerMode savedMode;
    AbstractPattern *savedPattern;

    Mixer *mixer;
    RandomPatternList *list;

    SolidColor *offColor;

    PlayerPowerState powerState = PLAYER_POWERED_OFF;
};

#endif //RGBWPLAY_PLAYER_H
