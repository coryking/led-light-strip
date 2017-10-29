//
// Created by Cory King on 10/29/17.
//

#ifndef RGBWPLAY_MIRRORPATTERN_H
#define RGBWPLAY_MIRRORPATTERN_H

#include <vector>
#include "AbstractPattern.h"
#include "FirePattern.h"


typedef struct {
    AbstractPattern* pattern;
    uint16 offset;
    uint16 length;

} LightString;

template <class T> class MirrorPattern : public AbstractPattern {
private:
    //std::vector<LightString> _lightStrings;
    ReversablePattern* leftPattern;
    ReversablePattern* rightPattern;
    uint16_t middleLedPosition;
public:
    virtual uint16_t readFrame(CRGB *buffer, ulong time) {
        /* for (auto & element : _lightStrings) {
            CRGB* bufferPos = buffer + element.offset;
            element.pattern->readFrame(bufferPos, time);
        }

        return getNumLeds(); */
        CRGB* bufferPos = buffer;
        leftPattern->readFrame(bufferPos, time);

        bufferPos = buffer + middleLedPosition;
        rightPattern->readFrame(bufferPos, time);

        return getNumLeds();

    }

    MirrorPattern(uint16 numLeds) : AbstractPattern(numLeds) {
        middleLedPosition = numLeds / 2;
        leftPattern = new T(middleLedPosition, false);
        rightPattern = new T(middleLedPosition, true);
    }
};


#endif //RGBWPLAY_MIRRORPATTERN_H
