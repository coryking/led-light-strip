//
// Created by Cory King on 12/3/17.
//

#include "NoisePlusPalette.h"

NoisePlusPalette::NoisePlusPalette(uint16 numLeds) : AbstractPattern(numLeds) {
    kMatrixWidth=numLeds;

    // Initialize our coordinates to some random values
    x = random16();
    y = random16();
    z = random16();
}

NoisePlusPalette::~NoisePlusPalette() {
    endAnimation();
}

void NoisePlusPalette::fillnoise8() {
    if(noise==NULL)
        return;
// If we're runing at a low "speed", some 8-bit artifacts become visible
    // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
    // The amount of data smoothing we're doing depends on "speed".
    uint8_t dataSmoothing = 0;
    if( speed < 50) {
        dataSmoothing = 200 - (speed * 4);
    }

    for(int i = 0; i < kMatrixWidth; i++) {
        int ioffset = scale * i;
        for(int j = 0; j < kMatrixWidth; j++) {
            int joffset = scale * j;

            uint8_t data = inoise8(x + ioffset,y + joffset,z);

            // The range of the inoise8 function is roughly 16-238.
            // These two operations expand those values out to roughly 0..255
            // You can comment them out if you want the raw noise data.
            data = qsub8(data,16);
            data = qadd8(data,scale8(data,39));

            if( dataSmoothing ) {
                uint8_t olddata = *(noise + i*kMatrixWidth + j);
                uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
                data = newdata;
            }

            *(noise + i*kMatrixWidth + j) = data;
        }
    }

    z += speed;

    // apply slow drift to X and Y, just for visual variation.
    x += speed / 8;
    y -= speed / 16;
}

uint16_t NoisePlusPalette::readFrame(CRGB *buffer, ulong time) {
    if(noise==NULL)
        return 0;

    ChangePaletteAndSettingsPeriodically();
    fillnoise8();
    static uint8_t ihue=0;

    for(int i = 0; i < kMatrixWidth; i++) {
        for(int j = 0; j < kMatrixHeight; j++) {
            // We use the value at the (i,j) coordinate in the noise
            // array for our brightness, and the flipped value from (j,i)
            // for our pixel's index into the color palette.

            uint8_t index = *(noise + j*kMatrixWidth + i); //noise[j][i];
            uint8_t bri =   *(noise + i*kMatrixWidth + j); //noise[i][j];

            // if this palette is a 'loop', add a slowly-changing base value
            if( colorLoop) {
                index += ihue;
            }

            // brighten up, as the color palette itself often contains the
            // light/dark dynamic range desired
            if( bri > 127 ) {
                bri = 255;
            } else {
                bri = dim8_raw( bri * 2);
            }

            CRGB color = ColorFromPalette( currentPalette, index, bri);
            *(buffer + XY(i,j)) = color;
        }
    }

    ihue+=1;
}

uint16_t NoisePlusPalette::XY(uint8_t x, uint8_t y) {
    uint16_t i;
    if( kMatrixSerpentineLayout == false) {
        i = (y * kMatrixWidth) + x;
    }
    if( kMatrixSerpentineLayout == true) {
        if( y & 0x01) {
            // Odd rows run backwards
            uint8_t reverseX = (kMatrixWidth - 1) - x;
            i = (y * kMatrixWidth) + reverseX;
        } else {
            // Even rows run forwards
            i = (y * kMatrixWidth) + x;
        }
    }
    return i;
}

void NoisePlusPalette::ChangePaletteAndSettingsPeriodically() {
    uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         speed = 20; scale = 30; colorLoop = 1; }
        if( secondHand ==  5)  { SetupPurpleAndGreenPalette();             speed = 10; scale = 50; colorLoop = 1; }
        if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       speed = 20; scale = 30; colorLoop = 1; }
        if( secondHand == 15)  { currentPalette = ForestColors_p;          speed =  8; scale =120; colorLoop = 0; }
        if( secondHand == 20)  { currentPalette = CloudColors_p;           speed =  4; scale = 30; colorLoop = 0; }
        if( secondHand == 25)  { currentPalette = LavaColors_p;            speed =  8; scale = 50; colorLoop = 0; }
        if( secondHand == 30)  { currentPalette = OceanColors_p;           speed = 20; scale = 90; colorLoop = 0; }
        if( secondHand == 35)  { currentPalette = PartyColors_p;           speed = 20; scale = 30; colorLoop = 1; }
        if( secondHand == 40)  { SetupRandomPalette();                     speed = 20; scale = 20; colorLoop = 1; }
        if( secondHand == 45)  { SetupRandomPalette();                     speed = 50; scale = 50; colorLoop = 1; }
        if( secondHand == 50)  { SetupRandomPalette();                     speed = 90; scale = 90; colorLoop = 1; }
        if( secondHand == 55)  { currentPalette = RainbowStripeColors_p;   speed = 30; scale = 20; colorLoop = 1; }
    }
}

void NoisePlusPalette::beginAnimation() {
    AbstractPattern::beginAnimation();
    if(noise==NULL)
        noise = (uint8_t *)malloc(sizeof(uint8_t) * kMatrixWidth * 2);
}

void NoisePlusPalette::endAnimation() {
    AbstractPattern::endAnimation();
    if(noise!=NULL)
        free(noise);

    noise=NULL;
}
