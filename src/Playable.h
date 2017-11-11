//
// Created by Cory King on 11/6/17.
//

#ifndef RGBWPLAY_PLAYABLE_H
#define RGBWPLAY_PLAYABLE_H

#include <Arduino.h>
#include <FastLED.h>


class Playable {
public:
    Playable(uint16_t numLeds) : _numLeds(numLeds) {}
    /**
 * Read pixels from this AbstractPattern
 * @param buffer Buffer to read into
 * @param time Timestamp, in milliseconds, when this was called
 * @return Number of bytes read
 */
    virtual uint16_t readFrame(CRGB * buffer, ulong time) = 0;

    uint16 getNumLeds() const {
        return _numLeds;
    }

private:
    uint16_t _numLeds;
};


#endif //RGBWPLAY_PLAYABLE_H
