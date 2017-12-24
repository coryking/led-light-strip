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
            this->readSteadyStateFrame(buffer, time);
            break;
    }

    return getNumLeds();
}

void Mixer::readSteadyStateFrame(CRGB* buffer, ulong time) {
    this->currentPattern->readFrame(buffer, time);
}

void Mixer::readTransitionFrame(CRGB*buffer, ulong time) {
    fract8 amountRemaining = map(time, transitionStartTime, transitionEndTime, 0, 255);
    currentPattern->readFrame(buffer, time);
    oldPattern->readFrame(this->oldPatternBuffer, time);

    nblend(buffer, this->oldPatternBuffer, getNumLeds(), ease8InOutApprox(255-amountRemaining));

    if(amountRemaining == 255 || time > transitionEndTime) {
        this->transitionToSteadyState();
    }
}

Mixer::Mixer(uint16_t numLeds) : Playable(numLeds) {
    this->currentPattern = new SolidColor(numLeds);
    this->oldPatternBuffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
}

void ICACHE_FLASH_ATTR Mixer::setNextPattern(AbstractPattern *nextPattern, uint16_t transitionTime) {
    syslog.log(LOG_INFO, "setting next pattern...");
    if(nextPattern == NULL) {
        return;
    }
    if(nextPattern == currentPattern) {
        syslog.log(LOG_DEBUG, "moving to new variant...");
        currentPattern->newVariant();
        return;
    }

    this->oldPattern = this->currentPattern;
    this->currentPattern = nextPattern;
    this->currentPattern->resetRuntime();

    //memcpy(this->oldPatternBuffer, this->currentPatternBuffer, sizeof(CRGB) * getNumLeds());

    this->transitionStartTime = millis();
    this->transitionEndTime = this->transitionStartTime + transitionTime;
    syslog.log(LOG_INFO, "ss to t");

    this->state = MIXER_TRANSITIONING;
}

Mixer::~Mixer() {
    free(this->oldPatternBuffer);
}

void ICACHE_FLASH_ATTR Mixer::transitionToSteadyState() {
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
