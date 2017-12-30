//
// Created by Cory King on 1/24/17.
//

#ifndef NEOPIXELLEDCONTROLLER_ABSTRACTPATTERN_H
#define NEOPIXELLEDCONTROLLER_ABSTRACTPATTERN_H

#include <Arduino.h>
#include <FastLED.h>
#include "Playable.h"

#define DEFAULT_DURATION (3 * 60 * 1000)

#define FIRST_LED_OFFSET 1
#define FIRST_LED_LENGTH 56
#define WEIRD_PART_OFFSET (FIRST_LED_LENGTH + FIRST_LED_OFFSET)
#define WEIRD_PART_LENGTH 9
#define VERTICAL_PART_OFFSET (WEIRD_PART_OFFSET + WEIRD_PART_LENGTH)
#define VERTICAL_LENGTH 36

#define FIRE_ESCAPE_OFFSET (VERTICAL_PART_OFFSET + VERTICAL_LENGTH)
#define CORNER_1_LENGTH 38
#define CORNER_1 (FIRE_ESCAPE_OFFSET + CORNER_1_LENGTH)

#define MIDDLE_LENGTH 95

#define CORNER_2 (MIDDLE_LENGTH + CORNER_1)

class AbstractPattern : public Playable {
private:
    ulong _startTime;

public:

    /**
     * Read pixels from this AbstractPattern
     * @param buffer Buffer to read into
     * @param time Timestamp, in milliseconds, when this was called
     * @return Number of bytes read
     */
    //virtual uint16_t readFrame(CRGB * buffer, ulong time) = 0;

    explicit AbstractPattern(uint16 numLeds) : Playable(numLeds) {
        _startTime = millis();
    }

    virtual void resetRuntime();

    ulong getRuntime();
    /**
     * Determines if this object can be stopped (i.e. has this made it past minRunTime?)
     * @return returns true if you can stop this
     */
    bool canStop();

    /**
         * Get the minimum milliseconds this can run for
         * @return returns minimum milliseconds this can run for
         */
    virtual uint16_t getMinRuntime() {
        return DEFAULT_DURATION;
    }

    /***
     * start the animation sequence.  Load any memory, etc...
     */
    virtual void beginAnimation() {}

    /***
     * end the animation sequence.  free any memory, etc...
     */
    virtual void endAnimation() {}


    /***
     * Switch to a new variation of this pattern.....
     */
    virtual void newVariant() {}
};

class HuePattern : public AbstractPattern {
private:
    uint8_t gHue = 0;
    const uint16_t minHueDelay = 15;
    const uint16_t maxHueDelay = 100;

    // rotating "base color" used by many of the patterns
    uint8_t gHueDelay = 20;
    void newHueDelay() { gHueDelay = random(minHueDelay, maxHueDelay); }
protected:

    virtual uint8_t getHue() const {
        return gHue;
    }

public:
    uint16_t readFrame(CRGB *buffer, ulong time) override {
        EVERY_N_MILLISECONDS(gHueDelay) {
            gHue++;
        }
    }

    void resetRuntime() override {
        newHueDelay();

        AbstractPattern::resetRuntime();
    }

    void newVariant() override {

    }

    explicit HuePattern(uint16 numLeds) : AbstractPattern(numLeds) {
        gHue = random(0,255);
        newHueDelay();
    }
};

#endif //NEOPIXELLEDCONTROLLER_ABSTRACTPATTERN_H
