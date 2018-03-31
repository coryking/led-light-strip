//
// Created by Cory King on 10/29/17.
//

#ifndef RGBWPLAY_MIRRORPATTERN_H
#define RGBWPLAY_MIRRORPATTERN_H

#include <FastLED.h>
#include "AbstractPattern.h"
#include "mem_functions.h"


template <class PATTERN_TYPE, int SEGMENTS=2> class MirrorPattern : public AbstractPattern {
private:
    AbstractPattern* pattern;
    CRGB* segmentBuffer;
    const int segmentNumLeds;
public:


    uint16_t readFrame(CRGB *buffer, ulong time) override {
        if(segmentBuffer == nullptr || pattern == nullptr)
            return 0;


        this->pattern->readFrame(segmentBuffer, time);

        bool invert = false;
        for(int segment=0; segment < SEGMENTS; segment++)
        {
            reverse_memcpy<CRGB>(&buffer[segmentNumLeds * segment], segmentBuffer, segmentNumLeds, invert);
            invert = !invert;
        }

        return getNumLeds();

    }

    void resetRuntime() override {
        this->pattern->resetRuntime();
    }

    ulong getRuntime() override {
        return this->pattern->getRuntime();
    }

    uint16_t getMinRuntime() override {
        return this->pattern->getMinRuntime();
    }

    void beginAnimation() override {
        AbstractPattern::beginAnimation();
        if(segmentBuffer == nullptr) {
            segmentBuffer = (CRGB*)malloc(sizeof(CRGB) * segmentNumLeds );
        }
        this->pattern->beginAnimation();
    }

    void endAnimation() override {
        AbstractPattern::endAnimation();
        if(segmentBuffer != nullptr) {
            free(segmentBuffer);
            segmentBuffer = nullptr;
        }
        this->pattern->endAnimation();
    }

    void newVariant() override {
        AbstractPattern::newVariant();
        this->pattern->newVariant();
    }

    explicit MirrorPattern(uint16 numLeds) :
            AbstractPattern(numLeds),
            segmentNumLeds(numLeds/SEGMENTS) {
        pattern = new PATTERN_TYPE(segmentNumLeds);


    }

    virtual ~MirrorPattern() {
        endAnimation();
    }
};


#endif //RGBWPLAY_MIRRORPATTERN_H
