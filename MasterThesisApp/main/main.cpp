
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"

extern "C" {
void app_main();
}

void app_main(void)
{
    LCD Lcd = LCD();
    for(;;){
        float s=3;
        int a=1;
    }
}
