//
// Created by Cory King on 11/11/17.
//

#include "Mixer.h"

uint16_t Mixer::readFrame(CRGB *buffer, ulong time) {
    switch(state) {
        case MIXER_TRANSITIONING:
            this->readTransitionFrame(buffer, time);
            break;
        case MIXER_STEADY_STATE:
            this->readSteadyStateFrame(buffer, time);
            break;
    }

    return getNumLeds();
}

void Mixer::readSteadyStateFrame(CRGB *buffer, ulong time) {
    this->currentPattern->readFrame(buffer, time);
}

void Mixer::readTransitionFrame(CRGB *buffer, ulong time) {
    fract8 amountRemaining = map(time, transitionStartTime, transitionEndTime, 0, 255);
    currentPattern->readFrame(buffer, time);
    nextPattern->readFrame(nextPatternLeds, time);

    nblend(buffer, nextPatternLeds, getNumLeds(), ease8InOutApprox(amountRemaining));

    if(amountRemaining == 255) {
        this->transitionToSteadyState();
    }
}

Mixer::Mixer(uint16_t numLeds) : Playable(numLeds) {
    this->currentPattern = new SolidColor(getNumLeds());
    this->nextPatternLeds = (CRGB*)malloc(sizeof(CRGB) * numLeds);
}

void Mixer::setNextPattern(AbstractPattern *nextPattern, uint16_t transitionTime) {
    if(nextPattern == NULL || nextPattern == currentPattern)
        return;

    this->nextPattern = nextPattern;
    this->transitionStartTime = millis();
    this->transitionEndTime = this->transitionStartTime + transitionTime;
    this->state = MIXER_TRANSITIONING;
}

Mixer::~Mixer() {
    free(this->nextPatternLeds);
}

void Mixer::transitionToSteadyState() {
    this->currentPattern = this->nextPattern;
    this->nextPattern = NULL;
    this->state = MIXER_STEADY_STATE;
}

AbstractPattern *Mixer::getCurrentPattern() const {
    return currentPattern;
}
