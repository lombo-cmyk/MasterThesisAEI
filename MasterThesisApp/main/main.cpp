
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/LCD.h"
#include "include/InterruptHandler.h"
#include "include//I2CWrapper.h"
#include "include/PressureSensor.h"
#include "include/ParticlesSensor.h"
#include <iostream>
#include "esp_log.h"

extern "C" {
void app_main();
}

void app_main(void) {
    vTaskDelay(SECOND / 10);
    double CO = 0, CO2 = 0, temperature = 0, humidity = 0;
    std::uint16_t PM25 = 0, PM10 = 0;
    unsigned int pressure = 0;
    auto& intHandler = InterruptHandler::getInstance();
    intHandler.InitializeInterrupts();
    auto& I2cWrapper = I2CWrapper::getInstance();
    I2cWrapper.ConfigureCommunication();
    LCD Lcd = LCD();
    PressureSensor pressureSensor = PressureSensor();
    ParticlesSensor ps = ParticlesSensor();
    ps.StartMeasuring(false);
    for (;;) {
        pressureSensor.PerformReadOut();
        pressureSensor.EnableOneMeasure();
        Lcd.GetCurrentMeasurements(ps.GetPM25(),
                                   ps.GetPM10(),
                                   CO,
                                   CO2,
                                   pressureSensor.GetTemperature(),
                                   humidity,
                                   pressureSensor.GetPressure());
        Lcd.DisplayCurrentState();
        ps.PerformReadout();
        vTaskDelay(SECOND / 10);
    }
}
