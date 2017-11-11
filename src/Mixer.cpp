//
// Created by Cory King on 11/11/17.
//

#include "Mixer.h"

uint16_t Mixer::readFrame(CRGB *buffer, ulong time) {
    switch(state) {
        case MIXER_TRANSITIONING:
            this->readTransitionFrame(time);
            break;
        case MIXER_STEADY_STATE:
            this->readSteadyStateFrame(time);
            break;
    }

    memcpy(buffer, currentPatternBuffer, sizeof(CRGB) * getNumLeds());

    return getNumLeds();
}

void Mixer::readSteadyStateFrame(ulong time) {
    this->currentPattern->readFrame(this->currentPatternBuffer, time);
}

void Mixer::readTransitionFrame(ulong time) {
    fract8 amountRemaining = 255 - map(time, transitionStartTime, transitionEndTime, 0, 255);
    currentPattern->readFrame(this->currentPatternBuffer, time);
    oldPattern->readFrame(this->oldPatternBuffer, time);

    nblend(this->currentPatternBuffer, this->oldPatternBuffer, getNumLeds(), ease8InOutApprox(amountRemaining));

    if(amountRemaining == 0) {
        this->transitionToSteadyState();
    }
}

Mixer::Mixer(uint16_t numLeds) : Playable(numLeds) {
    this->currentPattern = new SolidColor(getNumLeds());
    this->oldPatternBuffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
    this->currentPatternBuffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
}

void Mixer::setNextPattern(AbstractPattern *nextPattern, uint16_t transitionTime) {
    if(nextPattern == NULL || nextPattern == currentPattern)
        return;

    this->oldPattern = this->currentPattern;
    this->currentPattern = nextPattern;
    this->currentPattern->resetRuntime();

    memcpy(this->oldPatternBuffer, this->currentPatternBuffer, sizeof(CRGB) * getNumLeds());

    this->transitionStartTime = millis();
    this->transitionEndTime = this->transitionStartTime + transitionTime;
    this->state = MIXER_TRANSITIONING;
}

Mixer::~Mixer() {
    free(this->oldPatternBuffer);
    free(this->currentPatternBuffer);
}

void Mixer::transitionToSteadyState() {
    this->oldPattern = NULL;
    this->state = MIXER_STEADY_STATE;
}

AbstractPattern *Mixer::getCurrentPattern() const {
    return currentPattern;
}

bool Mixer::canStop() const {
    return this->currentPattern->canStop();
}
