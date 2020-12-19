
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"
#include "include/InterruptHandler.h"
#include "include//I2CWrapper.h"
#include <iostream>

extern "C" {
void app_main();
}

void app_main(void)
{
    InterruptHandler::Start();
    I2CWrapper I2CBus = I2CWrapper();
    LCD Lcd = LCD();
    for(;;){
        std::cout << "Display state: " << InterruptHandler::GetDisplayState() << std::endl;
        Lcd.DisplayCurrentState();
    }
}
