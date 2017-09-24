//
// Created by Cory King on 9/23/17.
//

#include "RandomPattern.h"

uint16_t RandomPattern::readFrame(CRGB *buffer, ulong time) {
    if(gCurrentPattern == NULL) gCurrentPattern = nextPattern();

    // Call the current pattern function once, updating the 'leds' array
    gCurrentPattern->readFrame(buffer, time);

    if(gTargetPattern) {
        if(time > startTransitionTime + TRANSITION_DURATION) {
            // we are at the end of the transition...
            // gotta black out the buffer....
            fill_solid(buffer, getNumLeds(), CRGB::Black);
            gTargetPattern->readFrame(buffer, time);
            gCurrentPattern = gTargetPattern;
            gCurrentPattern->resetRuntime();
            gTargetPattern = NULL;
        } else {
            gTargetPattern->readFrame(targetLeds, time);

            if(time > startTransitionTime + TRANSITION_DURATION / 2) {
                // we need to fade out the original...
                const ulong fade_out_amount = map(time + TRANSITION_DURATION / 2, startTransitionTime, startTransitionTime + TRANSITION_DURATION /2 , 0, 255);
                fadeToBlackBy(buffer, getNumLeds(), fade_out_amount);
            } else {
                // we need to fade in the new guy....
                const ulong fade_in_amount = map(time, startTransitionTime, startTransitionTime + TRANSITION_DURATION /2 , 0, 255);
                fadeToBlackBy(targetLeds, getNumLeds(), 255 - fade_in_amount);
            }

            nblend(buffer, targetLeds, getNumLeds(), 255);
        }
    } else if(gCurrentPattern->canStop()) {
        AbstractPattern* theNextPattern = nextPattern();
        if(theNextPattern != gCurrentPattern) {
            startTransitionTime = time;
            gTargetPattern = theNextPattern;
            fill_solid(targetLeds, getNumLeds(), CRGB::Black);
        }
    }
}

RandomPattern::RandomPattern(uint16 numLeds) : AbstractPattern(numLeds) {
    targetLeds = (CRGB*)malloc(sizeof(CRGB) * numLeds);

    gPatterns.push_back(new Rainbow(numLeds));
    gPatterns.push_back(new RollingPattern(numLeds, RainbowColors_p));
    gPatterns.push_back(new RollingPattern(numLeds, RainbowStripeColors_p));
    gPatterns.push_back(new Sinelon(numLeds));
    gPatterns.push_back(new Confetti(numLeds));
    gPatterns.push_back(new Noise(numLeds));
    gPatterns.push_back(new JugglePattern(numLeds));
    gPatterns.push_back(new RollingPattern(numLeds, OceanColors_p));
    gPatterns.push_back(new RollingPattern(numLeds, CloudColors_p));
    gPatterns.push_back(new RollingPattern(numLeds, ForestColors_p));
}

RandomPattern::~RandomPattern() {
    free(targetLeds);
}

AbstractPattern *RandomPattern::nextPattern() {
    uint8_t pattern = random(0, gPatterns.size());
    gPatterns[pattern]->resetRuntime();
    return gPatterns[pattern];
}
