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
#define DATA_PIN D2

#endif
#ifdef STRIP_BABYROOM
/* for the baby room
 main strip (180) + side wall (58)
 */
#    warning "Babyroom Strip"

#define NUM_LEDS 238
#define FASTLED_RGBW
#define FASTLED_ESP8266_DMA

#define DATA_PIN D2
#define LED_TYPE SK6812
//#define DEBUG_LOOP_TIMING
#endif

#ifdef STRIP_BEDROOM
/* for the bedroom
 */
#    warning "Bedroom Strip"

#define NUM_LEDS 180
#define FASTLED_RGBW
#define FASTLED_ESP8266_DMA
#define DATA_PIN D2
#define LED_TYPE SK6812
#define HAS_IR_REMOTE
#define IR_REMOTE_PIN D5
#define DEBUG_LOOP_TIMING
#endif

#ifdef STRIP_OUTSIDE

// Outside lights
#    warning "Outside Strip"

#define NUM_LEDS    300
#define DATA_PIN D2
#define LED_TYPE WS2812B
#define USE_FIRE_PATTERNS
//#define DEBUG_LOOP_TIMING

#endif
#endif //RGBWPLAY_DEVICES_H
