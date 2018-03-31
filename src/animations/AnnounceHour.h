//
// Created by Cory King on 1/25/17.
//

#ifndef NEOPIXELLEDCONTROLLER_ANNOUNCEHOUR_H
#define NEOPIXELLEDCONTROLLER_ANNOUNCEHOUR_H

#include "AbstractPattern.h"

// Run for exactly 60 seconds
#define ANNOUNCE_RUNTIME (30 * 1000)

class AnnounceHour: public AbstractPattern {
public:
    uint16_t readFrame(CRGB *buffer, ulong time) override;

    explicit AnnounceHour(uint16 numLeds);

private:
    uint16_t getMinRuntime() override {
        return ANNOUNCE_RUNTIME;
    }

};


#endif //NEOPIXELLEDCONTROLLER_ANNOUNCEHOUR_H
