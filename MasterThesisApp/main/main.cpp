
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"
#include "include/InterruptHandler.h"
#include "include//I2CWrapper.h"
#include "include/PressureSensor.h"
#include <iostream>
#include "esp_log.h"

extern "C" {
void app_main();
}

void app_main(void) {
    vTaskDelay(SECOND/10);
    double PM25 = 0, PM10 = 0, CO = 0, CO2 = 0, temperature = 0,
        humidity = 0;
    unsigned int pressure = 0;
    auto& intHandler = InterruptHandler::getInstance();
    intHandler.InitializeInterrupts();
    auto& I2cWrapper = I2CWrapper::getInstance();
    I2cWrapper.ConfigureCommunication();
    LCD Lcd = LCD();
    PressureSensor pressureSensor = PressureSensor();
    for (;;) {
        pressureSensor.PerformReadOut();
        pressureSensor.EnableOneMeasure();
        pressure = pressureSensor.GetPressure();
        temperature = pressureSensor.GetTemperature();
        Lcd.GetCurrentMeasurements(PM25, PM10, CO, CO2, temperature, humidity, pressure);
        Lcd.DisplayCurrentState();
    }
}
