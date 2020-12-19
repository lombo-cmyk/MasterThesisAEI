//
// Created by lukaszk on 19.12.2020.
//

#include "include/I2CWrapper.h"
#include "include/Definitions.h"

void I2CWrapper::ConfigureConnectionDetails() {
    connectionConfiguration_.mode = I2C_MODE_MASTER;
    connectionConfiguration_.sda_io_num = LCD_SDA_PIN;
    connectionConfiguration_.scl_io_num = LCD_SCL_PIN;
    connectionConfiguration_.sda_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.scl_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &connectionConfiguration_);
    i2c_driver_install(I2C_NUM_0, connectionConfiguration_.mode, 0, 0, 0);
}

void I2CWrapper::ConfigureDisplaySettings() {
    auto smbus_info = new smbus_info_t;
    smbus_init(smbus_info, i2cInterface_, displayAddress_);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);
    smBusInfoDisplay_ = smbus_info;
}
void I2CWrapper::ConfigureCommunication() {
    I2CWrapper::ConfigureConnectionDetails();
    I2CWrapper::ConfigureDisplaySettings();
}
