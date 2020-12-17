
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"
#include "include/InterruptHandler.h"
#include <iostream>

extern "C" {
void app_main();
}

void app_main(void)
{
    InterruptHandler::Start();
    LCD Lcd = LCD();
    for(;;){
        std::cout << "Display state: " << InterruptHandler::GetDisplayState() << std::endl;
        Lcd.DisplayCurrentState();
    }
}
