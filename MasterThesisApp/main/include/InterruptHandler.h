//
// Created by lukaszk on 12.07.2020.
//

#ifndef AIRCONTROLLER_INTERRUPTHANDLER_H
#define AIRCONTROLLER_INTERRUPTHANDLER_H

#include "Definitions.h"
#include "Singleton.h"
#include "freertos/FreeRTOS.h"
#include "freertos//task.h"
#include <cstdint>

class InterruptHandler final : public Singleton<InterruptHandler> {
public:
    friend Singleton<InterruptHandler>;
    void InitializeInterrupts();
    std::uint16_t GetDisplayState();
    auto SetBacklightFromLcd() -> bool& {
        return lcdBacklight_;
    }
    auto GetLcdBacklight() -> const bool& {
        return lcdBacklight_;
    }

private:
    std::uint16_t displayState_ = 0;
    bool lcdBacklight_ = true;
    TickType_t lastWakeTimeForwardButton_ = xTaskGetTickCount();
    const std::uint16_t maxDisplayStates_ = 5;
    static void DisplayNextState(void* arg);
};

#endif // AIRCONTROLLER_INTERRUPTHANDLER_H
