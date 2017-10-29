//
// Created by Cory King on 10/29/17.
//

#ifndef RGBWPLAY_DEVICES_H
#define RGBWPLAY_DEVICES_H

#ifdef STRIP_BATHROOM
// for the bathroom
#    warning "Bathroom strip"

//#define FASTLED_RGBW
#define NUM_LEDS 102
#define DATA_PIN 4

#define
#elif defined(STRIP_BABYROOM)
// for the baby room
#    warning "Babyroom Strip"

#define FASTLED_RGBW
#define NUM_LEDS 180
#define DATA_PIN 4
#define LED_TYPE SK6812

#elif defined(STRIP_OUTSIDE)

// Outside lights
#    warning "Outside Strip"

#define NUM_LEDS    300
#define DATA_PIN D2
#define LED_TYPE WS2812B
#define USE_FIRE_PATTERNS

#else

#error "Nothing Defined!"

#endif
#endif //RGBWPLAY_DEVICES_H
