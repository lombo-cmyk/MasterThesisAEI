
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

extern "C" {
void app_main();
}

void app_main(void) {
    double CO = 0;

//    adc1_config_width(ADC_WIDTH_BIT_12);
//    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
//    auto* adc_chars = static_cast<esp_adc_cal_characteristics_t*>(
//        malloc(sizeof(esp_adc_cal_characteristics_t)));
//    esp_adc_cal_characterize(ADC_UNIT_1,
//                             ADC_ATTEN_DB_11,
//                             ADC_WIDTH_BIT_12,
//                             ESP_ADC_CAL_VAL_DEFAULT_VREF,
//                             adc_chars);
    auto adc_chars = new esp_adc_cal_characteristics_t;
    ConfigureADC(adc_chars);
    int val = 0;
    std::uint64_t timeStart = 0, timeStop = 0;

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
        dht.ReadDHT();
        Lcd.GetCurrentMeasurements(
            ps.GetPM25(),
            ps.GetPM10(),
            CO,
            //                                   Co2.GetCo2Value(),
            Co2.GetCo2Value(),
            pressureSensor.GetTemperature(),
            dht.GetHumidity(),
            pressureSensor.GetPressure());
        Lcd.DisplayCurrentState();
        ps.PerformReadout();
        Co2.PerformReadout();
//        UpdateModbusRegistersDHT(dht.GetHumidity());
//        timeStart = esp_timer_get_time();
        val = adc1_get_raw(ADC1_CHANNEL_5);
//        std::uint32_t voltage = esp_adc_cal_raw_to_voltage(val, adc_chars);
//        timeStop = esp_timer_get_time();
        const int probes = 64;
        std::uint32_t average=0;
        auto voltage = new std::uint32_t;
        for (int i=0; i<probes; i++){
            esp_adc_cal_get_voltage(ADC_CHANNEL_5, adc_chars, voltage);
            average+=*voltage;
        }
        average/=probes;
        ESP_LOGI(deviceCoSens, "::::Measured CO level RAW:::: %d", val);
//        ESP_LOGI(deviceCoSens,
//                 "::::Measurement time:::: %llu",
//                 timeStop - timeStart);
        ESP_LOGI(deviceCoSens,
                 "::::Measured CO level VOLTAGE:::: %d",
                 average);

        vTaskDelay(SECOND);
    }
}
