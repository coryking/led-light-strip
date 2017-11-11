//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_MIXER_H
#define RGBWPLAY_MIXER_H

#include "Playable.h"
#include "animations/AbstractPattern.h"
#include "animations/SolidColor.h"

#define DEFAULT_TRANSITION_TIME 2000

enum MixerState {
    MIXER_STEADY_STATE = 0,
    MIXER_TRANSITIONING
};

class Mixer : public Playable {
public:
    void setNextPattern(AbstractPattern *nextPattern, uint16_t transitionTime = DEFAULT_TRANSITION_TIME);

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    AbstractPattern *getCurrentPattern() const;

    bool canStop() const;

    Mixer(uint16_t numLeds);

    virtual ~Mixer();

private:
    CRGB *oldPatternBuffer;
    CRGB *currentPatternBuffer;

    AbstractPattern *currentPattern = NULL;
    AbstractPattern *oldPattern = NULL;

    ulong transitionStartTime;
    ulong transitionEndTime;


    MixerState  state = MIXER_STEADY_STATE;

    void transitionToSteadyState();

    void readTransitionFrame(ulong time);

    void readSteadyStateFrame(ulong time);
};


#endif //RGBWPLAY_MIXER_H
