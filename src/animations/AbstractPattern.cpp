//
// Created by Cory King on 1/24/17.
//

#include "AbstractPattern.h"
#include <pixeltypes.h>


void AbstractPattern::resetRuntime() {
    _startTime = millis();
}

ulong AbstractPattern::getRuntime() {
    return millis() - _startTime;
}

bool AbstractPattern::canStop() {
    return getRuntime() >= getMinRuntime();
}
