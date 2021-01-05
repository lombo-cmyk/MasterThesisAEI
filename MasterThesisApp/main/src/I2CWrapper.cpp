//
// Created by lukaszk on 19.12.2020.
//

#include "include/I2CWrapper.h"
#include "include/Definitions.h"
#include "esp_log.h"
#include "smbus.h"
#include "driver/i2c.h"

void I2CWrapper::ConfigureConnectionDetails() {
    connectionConfiguration_.mode = I2C_MODE_MASTER;
    connectionConfiguration_.sda_io_num = SDA_PIN;
    connectionConfiguration_.scl_io_num = SCL_PIN;
    connectionConfiguration_.sda_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.scl_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &connectionConfiguration_);
    i2c_driver_install(I2C_NUM_0, connectionConfiguration_.mode, 0, 0, 0);
}

void I2CWrapper::ConfigureDisplayBusInfo() {
    auto smbus_info = new smbus_info_t;
    smbus_init(smbus_info, i2cInterface_, displayAddress_);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);
    smBusInfoDisplay_ = smbus_info;
}

void I2CWrapper::ConfigurePressureSensorBusInfo() {
    auto smbus_info = new smbus_info_t;
    smbus_init(smbus_info, i2cInterface_, pressureSensorAddress_);
    smbus_set_timeout(smbus_info, 10000 / portTICK_RATE_MS);
    smBusInfoPressureSensor_ = smbus_info;
}

void I2CWrapper::ConfigurePmSensorBusInfo() {
    auto smbus_info = new smbus_info_t;
    smbus_init(smbus_info, i2cInterface_, pmSensorAddress_);
    smbus_set_timeout(smbus_info, 10000 / portTICK_RATE_MS);
    smBusInfoPm_ = smbus_info;
}

void I2CWrapper::ConfigureCo2SensorBusInfo() {
    auto smbus_info = new smbus_info_t;
    smbus_init(smbus_info, i2cInterface_, Co2SensorAddress_);
    smbus_set_timeout(smbus_info, 10000 / portTICK_RATE_MS);
    smBusInfoCo2 = smbus_info;
}

bool I2CWrapper::isInit(const smbus_info_t* busInfo, const char* deviceName) {
    bool ok = false;
    if (busInfo != nullptr) {
        if (busInfo->init) {
            ok = true;
        } else {
            ESP_LOGE(deviceName, "Is not initialised");
        }
    } else {
        ESP_LOGE(deviceName, "Is NULL");
    }
    ESP_LOGI(deviceName, "Bus configured properly");
    return ok;
}
void I2CWrapper::ConfigureCommunication() {
    I2CWrapper::ConfigureConnectionDetails();

    I2CWrapper::ConfigureDisplayBusInfo();
    I2CWrapper::isInit(smBusInfoDisplay_, deviceLCD);
    I2CWrapper::pingDevice(smBusInfoDisplay_, deviceLCD);

    I2CWrapper::ConfigurePressureSensorBusInfo();
    I2CWrapper::isInit(smBusInfoPressureSensor_, devicePressSens);
    I2CWrapper::pingDevice(smBusInfoPressureSensor_, devicePressSens);

    I2CWrapper::ConfigurePmSensorBusInfo();
    I2CWrapper::isInit(smBusInfoPm_, devicePmSens);
    I2CWrapper::pingDevice(smBusInfoPm_, devicePmSens);

    I2CWrapper::ConfigureCo2SensorBusInfo();
    I2CWrapper::isInit(smBusInfoCo2, deviceCo2Sens);
    I2CWrapper::pingDevice(smBusInfoCo2, deviceCo2Sens);
}
void I2CWrapper::pingDevice(smbus_info_t* busInfo, const char* device) {
    esp_err_t error = smbus_quick(busInfo, I2C_MASTER_WRITE);
    if (error == ESP_OK) {
        ESP_LOGI(device, "I2C communication established");
    } else {
        ESP_LOGE(device,
                 "I2C communication not established, error: %d",
                 error);
    }
}
