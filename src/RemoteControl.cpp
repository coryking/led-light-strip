//
// Created by Cory King on 12/1/17.
//

#include "RemoteControl.h"

void RemoteControl::OnUpdate(uint32_t deltaTime) {
    Task::OnUpdate(deltaTime);
}

RemoteControl::RemoteControl() : Task(MsToTaskTime(100)) {}
