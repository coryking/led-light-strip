//
// Created by Cory King on 9/23/17.
//

#ifndef RGBWPLAY_RANDOMPATTERN_H
#define RGBWPLAY_RANDOMPATTERN_H

#include <FastLED.h>
#include "patterns.h"

#define TRANSITION_DURATION 4000


class RandomPattern : public AbstractPattern {
public:
    virtual uint16_t readFrame(CRGB *buffer, ulong time);

    RandomPattern(uint16 numLeds);

    virtual ~RandomPattern();

private:
    CRGB *targetLeds;

    ulong startTransitionTime;

// List of patterns to cycle through.  Each is defined as a separate function below.
    std::vector<AbstractPattern *> gPatterns;
    AbstractPattern * gCurrentPattern = NULL;
    AbstractPattern * gTargetPattern = NULL;

    AbstractPattern* nextPattern();
};


#endif //RGBWPLAY_RANDOMPATTERN_H
