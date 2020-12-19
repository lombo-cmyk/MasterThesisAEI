//
// Created by lukaszk on 12.07.2020.
//

#include "include/InterruptHandler.h"

std::uint16_t InterruptHandler::displayState_ = 0;
bool InterruptHandler::lcdBacklight_ = true;
TickType_t InterruptHandler::lastWakeTimeForwardButton_ = xTaskGetTickCount();

void InterruptHandler::DisplayNextState(void* arg) {
    if ((xTaskGetTickCount() - lastWakeTimeForwardButton_) > DEBOUNCE_TIME) {
        displayState_++;
        if (displayState_ > maxDisplayStates) {
            displayState_ = 0;
        }
        lastWakeTimeForwardButton_ = xTaskGetTickCount();
    }
}

void InterruptHandler::Start() {
    gpio_set_direction(FORWARD_BUTTON, GPIO_MODE_INPUT);
    gpio_set_direction(FORWARD_BUTTON, GPIO_MODE_INPUT);
    gpio_set_direction(BUTTON_2, GPIO_MODE_INPUT);
    gpio_set_intr_type(FORWARD_BUTTON, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(FORWARD_BUTTON, DisplayNextState, nullptr);
}

std::uint16_t InterruptHandler::GetDisplayState() {
    return displayState_;
}
