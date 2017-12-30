//
// Created by Cory King on 1/25/17.
//

#ifndef NEOPIXELLEDCONTROLLER_TESTPATTERN_H
#define NEOPIXELLEDCONTROLLER_TESTPATTERN_H

#include <FastLED.h>
#include "AbstractPattern.h"

#define EACH_TEST_TIME 1
#define NUM_TESTS 5
#define TEST_RUNTIME (EACH_TEST_TIME * NUM_TESTS * 1000)

class TestPattern : public AbstractPattern {
private:
    uint16_t getMinRuntime() override {
        return TEST_RUNTIME;
    }
public:
    explicit TestPattern(uint16 numLeds);

    uint16_t readFrame(CRGB *buffer, ulong time) override;
};


#endif //NEOPIXELLEDCONTROLLER_TESTPATTERN_H
