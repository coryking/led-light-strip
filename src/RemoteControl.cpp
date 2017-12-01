//
// Created by Cory King on 12/1/17.
//

#include "RemoteControl.h"

void RemoteControl::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);

    decode_results results;

    if(irrecv->decode(&results)) {
        serialPrintUint64(results.value);
        if(remoteButtonMap.find(results.value) != remoteButtonMap.end() &&
           remoteButtonPressedEvent != NULL) {
            RemoteButtons btn = remoteButtonMap[results.value];
            remoteButtonPressedEvent(btn);
        }
        irrecv->resume();
    }
}

RemoteControl::RemoteControl() : Task(MsToTaskTime(IR_CHECK_MS)) {}

void
RemoteControl::setRemoteButtonPressedEvent(const RemoteControl::RemoteButtonPressedEventFn remoteButtonPressedEvent) {
    RemoteControl::remoteButtonPressedEvent = remoteButtonPressedEvent;
}

void RemoteControl::initRemote(uint8_t pin) {
    irrecv = new IRrecv(pin);
    irrecv->enableIRIn();
}
