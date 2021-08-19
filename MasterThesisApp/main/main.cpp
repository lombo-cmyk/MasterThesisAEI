
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LCD.h"
#include "I2CWrapper.h"
#include "PressureSensor.h"
#include "ParticlesSensor.h"
#include "Co2Sensor.h"
#include "esp_log.h"
#include "DHT.h"
#include "EthernetW5500.h"
#include "Modbus.h"
#include <driver/adc.h>
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "AdcWrapper.h"
#include <cmath>
extern "C" {
void app_main();
}

void UpdateModbusRegistersCO(float Co_mVolts) {
    // TODO: move to other file
    auto& modbusManager = Modbus::getInstance();
    Co_mVolts-=20;
    float co_ppm = 1;
    if(Co_mVolts >= 1150){
        auto Co_volts = Co_mVolts/1000;
        co_ppm = round((Co_volts - 1.15) / 0.00206);
    }

    std::array<std::uint8_t, 2> index{indexCo_mVolts, indexCo_ppm};
    std::array<float, 2> value{Co_mVolts, co_ppm}; // this esp measures 20mV too much
    modbusManager.UpdateHoldingRegs(index, value);
}

void app_main(void) {
    float CO = 0;
    esp_log_level_set("*", ESP_LOG_INFO);
    auto adc_chars = new esp_adc_cal_characteristics_t;
    ConfigureADC(adc_chars);
    int val = 0;

    auto& intHandler = InterruptHandler::getInstance();
    intHandler.InitializeInterrupts();
    auto& I2cWrapper = I2CWrapper::getInstance();
    I2cWrapper.ConfigureCommunication();

    auto& ethManager = EthernetW5500::getInstance();
    ethManager.SelectSpiInterface(SPI2_HOST);
    ethManager.ConfigureAndStart();
    auto& modbusManager = Modbus::getInstance();

    LCD Lcd = LCD();
    Lcd.DisplayWelcomeMessage();
    PressureSensor pressureSensor = PressureSensor();
    pressureSensor.TurnDeviceOn();
    pressureSensor.EnableOneMeasure();
    ParticlesSensor ps = ParticlesSensor();
    ps.StartMeasuring(false);
    Co2Sensor Co2 = Co2Sensor();
    Co2.StartMeasuring();
    auto dht = DHT(DHT_PIN);

    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = nullptr;
    xTaskCreate(Modbus::RunSlaveTask,
                "Modbus_task",
                2048,
                &ucParameterToPass,
                5,
                &xHandle);

    for (;;) {
        pressureSensor.PerformReadOut();
        pressureSensor.EnableOneMeasure();
        if (dht.ReadDHT()==0){
            dht.UpdateModbusRegisters(indexHumidityDHT,
                                      indexTemperatureDHT);
        }
        Lcd.GetCurrentMeasurements(
            ps.GetPM25(),
            ps.GetPM10(),
            CO,
            Co2.GetCo2Value(),
            pressureSensor.GetTemperature(),
            dht.GetHumidity(),
            pressureSensor.GetPressure());
        Lcd.DisplayCurrentState();
        ps.PerformReadout();
        Co2.PerformReadout();
        val = adc1_get_raw(ADC1_CHANNEL_5);
        const int probes = 64;
        std::uint32_t average=0;
        std::uint32_t voltage = 0;
        for (int i=0; i<probes; i++){
            esp_adc_cal_get_voltage(ADC_CHANNEL_5, adc_chars, &voltage);
            average+=voltage;
        }
        average/=probes;
        CO=average;
        ESP_LOGI(deviceCoSens, "::::Measured CO level RAW:::: %d", val);
        ESP_LOGI(deviceCoSens,
                 "::::Measured CO level VOLTAGE:::: %d",
                 average);
        UpdateModbusRegistersCO(static_cast<float>(CO));
        ethManager.executeEthernetStatusGuard();
        vTaskDelay(SECOND);
    }
}
