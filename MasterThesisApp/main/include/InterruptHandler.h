//
// Created by lukaszk on 12.07.2020.
//

#ifndef AIRCONTROLLER_INTERRUPTHANDLER_H
#define AIRCONTROLLER_INTERRUPTHANDLER_H

#include "Definitions.h"
#include "freertos/FreeRTOS.h"
#include "freertos//task.h"
#include <cstdint>

struct InterruptHandler {
public:
    static void Start();
    static std::uint16_t GetDisplayState();
    static auto SetBacklightFromLcd() -> bool& {
        return lcdBacklight_;
    }
    static auto GetLcdBacklight() -> const bool& {
        return lcdBacklight_;
    }



private:
    static std::uint16_t displayState_;
    static bool lcdBacklight_;
    static TickType_t lastWakeTimeForwardButton_;

    static void DisplayNextState(void* arg);
};

#endif // AIRCONTROLLER_INTERRUPTHANDLER_H
