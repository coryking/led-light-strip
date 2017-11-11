//
// Created by Cory King on 11/11/17.
//

#include "Mixer.h"
#include "config.h"

uint16_t Mixer::readFrame(CRGB *buffer, ulong time) {
    switch(state) {
        case MIXER_TRANSITIONING:
            this->readTransitionFrame(buffer, time);
            break;
        case MIXER_STEADY_STATE:
            this->readSteadyStateFrame(time);
            memcpy(buffer, this->currentPatternBuffer, sizeof(CRGB) * getNumLeds());
            break;
    }

    return getNumLeds();
}

void Mixer::readSteadyStateFrame(ulong time) {
    this->currentPattern->readFrame(this->currentPatternBuffer, time);
}

void Mixer::readTransitionFrame(CRGB*buffer, ulong time) {
    fract8 amountRemaining = map(time, transitionStartTime, transitionEndTime, 0, 255);
    currentPattern->readFrame(buffer, time);
    oldPattern->readFrame(this->oldPatternBuffer, time);

    blend(this->oldPatternBuffer, this->currentPatternBuffer, buffer, getNumLeds(), ease8InOutApprox(amountRemaining));

    if(time > transitionEndTime) {
        this->transitionToSteadyState();
    }
}

Mixer::Mixer(uint16_t numLeds) : Playable(numLeds) {
    this->currentPattern = new SolidColor(numLeds);
    this->oldPatternBuffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
    this->currentPatternBuffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
}

void Mixer::setNextPattern(AbstractPattern *nextPattern, uint16_t transitionTime) {
    syslog.log(LOG_INFO, "setting next pattern...");
    if(nextPattern == NULL || nextPattern == currentPattern) {
        return;
    }

    this->oldPattern = this->currentPattern;
    this->currentPattern = nextPattern;
    this->currentPattern->resetRuntime();

    memcpy(this->oldPatternBuffer, this->currentPatternBuffer, sizeof(CRGB) * getNumLeds());

    this->transitionStartTime = millis();
    this->transitionEndTime = this->transitionStartTime + transitionTime;
    syslog.log(LOG_INFO, "ss to t");

    this->state = MIXER_TRANSITIONING;
}

Mixer::~Mixer() {
    free(this->oldPatternBuffer);
    free(this->currentPatternBuffer);
}

void Mixer::transitionToSteadyState() {
    syslog.log(LOG_INFO, "t to ss");
    this->oldPattern = NULL;
    this->state = MIXER_STEADY_STATE;
}

AbstractPattern *Mixer::getCurrentPattern() const {
    return currentPattern;
}

bool Mixer::canStop() const {
    return this->currentPattern->canStop();
}
