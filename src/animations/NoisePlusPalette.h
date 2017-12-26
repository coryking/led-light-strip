//
// Created by Cory King on 12/3/17.
//

#ifndef RGBWPLAY_NOISEPLUSPALETTE_H
#define RGBWPLAY_NOISEPLUSPALETTE_H

#include <FastLED.h>
#include "AbstractPattern.h"

#define HOLD_PALETTES_X_TIMES_AS_LONG 1

class NoisePlusPalette : public AbstractPattern {
public:
    NoisePlusPalette(uint16 numLeds);
    virtual ~NoisePlusPalette();

    uint16_t readFrame(CRGB *buffer, ulong time) override;

    void beginAnimation() override;

    void endAnimation() override;

protected:
    uint8_t kMatrixWidth  = 16;
    const uint8_t kMatrixHeight = 1;
    const bool    kMatrixSerpentineLayout = true;

    // The 16 bit version of our coordinates
    uint16_t x;
    uint16_t y;
    uint16_t z;


    // We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
    // use the z-axis for "time".  speed determines how fast time moves forward.  Try
    // 1 for a very slow moving effect, or 60 for something that ends up looking like
    // water.
    uint16_t speed = 20; // speed is set dynamically once we've started up

    // Scale determines how far apart the pixels in our noise matrix are.  Try
    // changing these values around to see how it affects the motion of the display.  The
    // higher the value of scale, the more "zoomed out" the noise iwll be.  A value
    // of 1 will be so zoomed in, you'll mostly see solid colors.
    uint16_t scale = 30; // scale is set dynamically once we've started up

    // This is the array that we keep our computed noise values in
    uint8_t *noise = NULL;

    CRGBPalette16 currentPalette;
    uint8_t       colorLoop = 1;

    void fillnoise8();

    uint16_t XY( uint8_t x, uint8_t y);

    void ChangePaletteAndSettingsPeriodically();

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is
// a bright hue, the third is a bright pastel, and the last is
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
    void SetupRandomPalette()
    {
        currentPalette = CRGBPalette16(
                CHSV( random8(), 255, 32),
                CHSV( random8(), 255, 255),
                CHSV( random8(), 128, 255),
                CHSV( random8(), 255, 255));
    }

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
    void SetupBlackAndWhiteStripedPalette()
    {
        // 'black out' all 16 palette entries...
        fill_solid( currentPalette, 16, CRGB::Black);
        // and set every fourth one to white.
        currentPalette[0] = CRGB::White;
        currentPalette[4] = CRGB::White;
        currentPalette[8] = CRGB::White;
        currentPalette[12] = CRGB::White;

    }

// This function sets up a palette of purple and green stripes.
    void SetupPurpleAndGreenPalette()
    {
        CRGB purple = CHSV( HUE_PURPLE, 255, 255);
        CRGB green  = CHSV( HUE_GREEN, 255, 255);
        CRGB black  = CRGB::Black;

        currentPalette = CRGBPalette16(
                green,  green,  black,  black,
                purple, purple, black,  black,
                green,  green,  black,  black,
                purple, purple, black,  black );
    }
};


#endif //RGBWPLAY_NOISEPLUSPALETTE_H
