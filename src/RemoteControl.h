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

class RemoteControl : public Task {
public:
    typedef std::function<void(int)> IntValueCallback;

    RemoteControl();

protected:
    void OnUpdate(uint32_t deltaTime) override;
};


#endif //RGBWPLAY_REMOTECONTROL_H
