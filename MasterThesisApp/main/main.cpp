
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/LCD.h"
#include "include//I2CWrapper.h"
#include "include/PressureSensor.h"
#include "include/ParticlesSensor.h"
#include "include//Co2Sensor.h"
#include "esp_log.h"
#include "DHT.h"
#include "EthernetW5500.h"
#include "Modbus.h"

extern "C" {
void app_main();
}

void app_main(void) {
    double CO = 0, humidity = 0;
    auto& intHandler = InterruptHandler::getInstance();
    intHandler.InitializeInterrupts();
    auto& I2cWrapper = I2CWrapper::getInstance();
    I2cWrapper.ConfigureCommunication();
    LCD Lcd = LCD();
    Lcd.DisplayWelcomeMessage();
    PressureSensor pressureSensor = PressureSensor();
    pressureSensor.TurnDeviceOn();
    pressureSensor.EnableOneMeasure();
    ParticlesSensor ps = ParticlesSensor();
    ps.StartMeasuring(false);
    Co2Sensor Co2 = Co2Sensor();
    Co2.StartMeasuring();
    auto dht = DHT();
    dht.setDHTgpio(DHT_PIN);
    auto& ethManager = EthernetW5500::getInstance();
    ethManager.SelectSpiInterface(SPI2_HOST);
    ethManager.ConfigureAndStart();
    auto& modbusManager = Modbus::getInstance();
    for (;;) {
        pressureSensor.PerformReadOut();
        pressureSensor.EnableOneMeasure();
        dht.readDHT();
        Lcd.GetCurrentMeasurements(ps.GetPM25(),
                                   ps.GetPM10(),
                                   CO,
//                                   Co2.GetCo2Value(),
                                   Co2.GetCo2Value(),
                                   pressureSensor.GetTemperature(),
                                   dht.getHumidity(),
                                   pressureSensor.GetPressure());
        Lcd.DisplayCurrentState();
        ps.PerformReadout();
        Co2.PerformReadout();
        vTaskDelay(SECOND);
    }
}
