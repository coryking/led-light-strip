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
                    { 0xFF38C7, BUTTON_0 },
            };

};


#endif //RGBWPLAY_REMOTECONTROL_H
