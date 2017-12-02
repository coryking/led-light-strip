//
// Created by Cory King on 12/1/17.
//

#ifndef RGBWPLAY_REMOTECONTROL_H
#define RGBWPLAY_REMOTECONTROL_H

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Task.h>
#include <functional>
#include <map>
#include "Commands.h"

#define IR_CHECK_MS 100

class RemoteControl : public Task {
public:
    typedef std::function<void(RemoteButtons)> RemoteButtonPressedEventFn;

    RemoteControl();

    void initRemote(uint8_t pin);

    void setRemoteButtonPressedEvent(const RemoteButtonPressedEventFn remoteButtonPressedEvent);

protected:
    void OnUpdate(uint32_t deltaTime) override;
private:
    RemoteButtonPressedEventFn remoteButtonPressedEvent;
    IRrecv *irrecv;
    std::map<uint64_t , RemoteButtons > remoteButtonMap =
            {
                    { 0xFF9867, BUTTON_0 },
                    { 0xFFA25D, BUTTON_1 },
                    { 0xFF629D, BUTTON_2 },
                    { 0xFFE21D, BUTTON_3 },
                    { 0xFF22DD, BUTTON_4 },
                    { 0xFF02FD, BUTTON_5 },
                    { 0xFFC23D, BUTTON_6 },
                    { 0xFFE01F, BUTTON_7 },
                    { 0xFFA857, BUTTON_8 },
                    { 0xFF906F, BUTTON_9 },
                    { 0xFF6897, BUTTON_ASTERISK },
                    { 0xFFB04F, BUTTON_POUND },
                    { 0xFF10EF, BUTTON_LEFT },
                    { 0xFF5AA5, BUTTON_RIGHT },
                    { 0xFF18E7, BUTTON_UP },
                    { 0xFF4AB5, BUTTON_DOWN },
                    { 0xFF38C7, BUTTON_OK },
            };

};


#endif //RGBWPLAY_REMOTECONTROL_H
