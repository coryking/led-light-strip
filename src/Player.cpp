//
// Created by Cory King on 11/11/17.
//

#include "Player.h"
#include "animations/patterns.h"

Player::Player(uint32_t numLeds, uint8_t framesPerSecond) :
        numLeds(numLeds),
        Task(MsToTaskTime(framesPerSecond/1000))
{
    buffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
    this->mixer = new Mixer(numLeds);
    this->list = new RandomPatternList({
#ifdef USE_FIRE_PATTERNS
                new MirrorPattern<FirePattern>(numLeds),
                new MirrorPattern<PalettePattern>(numLeds),
                new MirrorPattern<RainbowFirePattern>(numLeds),
#endif
               new Rainbow(numLeds),
               new RollingPattern(numLeds, RainbowColors_p),
               new RollingPattern(numLeds, RainbowStripeColors_p),
               new Sinelon(numLeds),
               new Confetti(numLeds),
               new Noise(numLeds),
               new JugglePattern(numLeds),
               new RollingPattern(numLeds, OceanColors_p),
               new RollingPattern(numLeds, CloudColors_p),
               new RollingPattern(numLeds, ForestColors_p),
       });
}

void Player::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);
    uint32_t time = millis();
    this->mixer->readFrame(this->getBuffer(), time);
    if(this->getMode() == Mode_RandomPattern) {
        if (this->mixer->getCurrentPattern()->canStop()) {
            this->mixer->setNextPattern(
                    this->list->getRandomPattern()
            );
        }
    }
}

PlayerMode Player::getMode() const {
    return mode;
}

void Player::setMode(PlayerMode mode) {
    Player::mode = mode;
}

Player::~Player() {
    free(this->buffer);
}

CRGB *Player::getBuffer() const {
    return buffer;
}

uint32_t Player::getNumLeds() const {
    return numLeds;
}
