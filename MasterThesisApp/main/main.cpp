
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"
#include "include/InterruptHandler.h"
#include "include//I2CWrapper.h"
#include <iostream>
#include "esp_log.h"

extern "C" {
void app_main();
}

void app_main(void) {
    vTaskDelay(SECOND/10);
    auto& intHandler = InterruptHandler::getInstance();
    intHandler.InitializeInterrupts();
    auto& I2cWrapper = I2CWrapper::getInstance();
    I2cWrapper.ConfigureCommunication();
    LCD Lcd = LCD();
    for (;;) {
        vTaskDelay(SECOND/10);
//        std::cout << "Display state: " << intHandler.GetDisplayState()
//                  << std::endl;
        Lcd.DisplayCurrentState();
    }
}
