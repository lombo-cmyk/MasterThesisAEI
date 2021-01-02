//
// Created by lukaszk on 12.07.2020.
//

#include "include/InterruptHandler.h"

void InterruptHandler::DisplayNextState(void* arg) {
    auto& instance = InterruptHandler::getInstance();
    if ((xTaskGetTickCount() - instance.lastWakeTimeForwardButton_) >
        DEBOUNCE_TIME) {
        instance.displayState_++;
        if (instance.displayState_ > instance.maxDisplayStates_) {
            instance.displayState_ = 0;
        }
        instance.lastWakeTimeForwardButton_ = xTaskGetTickCount();
    }
}

void InterruptHandler::InitializeInterrupts() {
    gpio_set_direction(FORWARD_BUTTON, GPIO_MODE_INPUT);
    gpio_set_direction(CO2_BUSY_BUTTON, GPIO_MODE_INPUT);
    gpio_set_intr_type(FORWARD_BUTTON, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(FORWARD_BUTTON, DisplayNextState, nullptr);
}

std::uint16_t InterruptHandler::GetDisplayState() {
    return displayState_;
}
