//
// Created by Cory King on 11/11/17.
//

#include "Player.h"
#include "animations/patterns.h"
#include "devices.h"

Player::Player(uint32_t numLeds, uint8_t framesPerSecond) :
        numLeds(numLeds),
        Task(MsToTaskTime(1000/framesPerSecond))
{
    offColor = new SolidColor(numLeds);
    buffer = (CRGB*)malloc(sizeof(CRGB) * numLeds);
    this->mixer = new Mixer(numLeds);
    this->list = new RandomPatternList({
           new NoisyConfetti(numLeds),

#ifdef USE_FIRE_PATTERNS
                new MirrorPattern<FirePattern>(numLeds),
                new MirrorPattern<PalettePattern>(numLeds),
                new MirrorPattern<RainbowFirePattern>(numLeds),
#endif
               new Rainbow(numLeds),
               new RollingPattern(numLeds, RainbowColors_p),
               new RollingPattern(numLeds, RainbowStripeColors_p),
               // new Sinelon(numLeds), // baby hates this one....
               new Confetti(numLeds),
               new Noise(numLeds),
               new JugglePattern(numLeds),
               new BurbleBabble(numLeds),
               new RollingPattern(numLeds, CloudColors_p),
               new BpmPattern(numLeds),
               //new NoisePlusPalette(numLeds),
                                               //new RollingPattern(numLeds, ForestColors_p),

       });
}

void Player::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);
    uint32_t time = millis();
#ifdef DEBUG_LOOP_TIMING
    uint32_t start_cyc = __clock_cycles();
#endif
    if(powerState == PLAYER_POWERING_ON) {
        this->mixer->setNextPattern(savedPattern, POWER_TRANSITION_MS);
        if(savedMode == Mode_RandomPattern) {
            this->setRandomMode();
        } else {
            this->setFixedPatternMode(savedPattern, POWER_TRANSITION_MS);
        }
        syslog.log(LOG_INFO, "POWER_ON");
        powerState = PLAYER_POWER_ON;
    }
    if(powerState == PLAYER_POWERING_OFF) {
        savedPattern = this->mixer->getCurrentPattern();
        savedMode = this->getMode();
        this->setFixedPatternMode(offColor, POWER_TRANSITION_MS);
        syslog.log(LOG_INFO, "POWER_OFF");
        powerState = PLAYER_POWERED_OFF;
    }
    if(powerState == PLAYER_POWER_ON || powerState == PLAYER_POWERED_OFF) {
        this->mixer->readFrame(this->getFastLEDBuffer(), time);
        if(this->getMode() == Mode_RandomPattern) {
            if (this->mixer->canStop()) {
                syslog.log(LOG_INFO, "next pattern...");
                this->mixer->setNextPattern(
                        this->list->getRandomPattern()
                );
            }
        }
    }
#ifdef DEBUG_LOOP_TIMING
    uint32_t reader_cyc = __clock_cycles();
#endif
    FastLED.show();
#ifdef DEBUG_LOOP_TIMING
    uint32_t done_cyc = __clock_cycles();

    EVERY_N_SECONDS_I(CYC_MEASURE,5) {
        syslog.logf(LOG_INFO, "sr %i rd %i sd %i", reader_cyc-start_cyc, done_cyc - reader_cyc, done_cyc - start_cyc);
    }
#endif

}

PlayerMode Player::getMode() const {
    return mode;
}

Player::~Player() {
    free(this->buffer);
}

CRGB *Player::getFastLEDBuffer() const {
    return buffer;
}

uint32_t Player::getNumLeds() const {
    return numLeds;
}

void Player::setPower(bool power) {
    if(power && powerState == PLAYER_POWERED_OFF) {
        syslog.log("Powering On");
        powerState = PLAYER_POWERING_ON;
    }
    if (!power && powerState == PLAYER_POWER_ON) {
        syslog.log("Powering Off");
        powerState = PLAYER_POWERING_OFF;
    }

}

bool Player::getPower() const {
    return (powerState == PLAYER_POWER_ON);
}

void Player::setRandomMode() {
    if(this->getMode() != Mode_RandomPattern) {
        this->mode = Mode_RandomPattern;
        this->mixer->setNextPattern(
                this->list->getRandomPattern()
        );
    }
}

void Player::setFixedPatternMode(AbstractPattern* pattern,  uint16_t transitionTime) {
    this->mode = Mode_FixedPattern;
    this->mixer->setNextPattern(pattern, transitionTime);
}

void Player::setPattern(uint8_t patternNumber) {
    if (patternNumber <= this->list->getNumPatterns()) {
        this->mode = Mode_FixedPattern;
        this->mixer->setNextPattern(
                this->list->getPattern(patternNumber)
        );
    }
}
