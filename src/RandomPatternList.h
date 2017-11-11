//
// Created by Cory King on 11/11/17.
//

#ifndef RGBWPLAY_RANDOMPATTERNLIST_H
#define RGBWPLAY_RANDOMPATTERNLIST_H

#include <FastLED.h>
#include <vector>
#include "animations/AbstractPattern.h"

typedef std::vector<AbstractPattern *> PatternList;

class RandomPatternList {
public:
    RandomPatternList() : RandomPatternList(PatternList()) {}

    RandomPatternList(PatternList patterns) : patterns(patterns) {

    }

    void addPattern(AbstractPattern* pattern) {
        this->patterns.push_back(pattern);
    }

    void addPatterns(PatternList list) {
        for(AbstractPattern* pattern : list) {
            this->patterns.push_back(pattern);
        }
    }
    AbstractPattern * getRandomPattern() {
        if(this->patterns.size() == 0)
            return NULL;

        uint8_t element = random8(this->patterns.size() - 1);
        return this->patterns[element];
    }
private:
    PatternList patterns;

};

#endif //RGBWPLAY_RANDOMPATTERNLIST_H
