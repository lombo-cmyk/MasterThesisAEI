//
// Created by lukaszk on 19.12.2020.
//

#include "include/PressureSensor.h"
#include "smbus.h"
#include <iostream>
#include <bitset>
PressureSensor::PressureSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    PressureCommunicationInfo_ = i2cWrapper.GetsmBusInfoPressure_();
}
void PressureSensor::ReadPressure() {
    esp_err_t err;
    err = smbus_read_byte(PressureCommunicationInfo_,
                          whoIAmReg_,
                          &dataFromSensor_);
    std::cout << "Error: " << err << std::endl;
    std::cout << "Dummy register data is: "
              << static_cast<std::bitset<8>>(dataFromSensor_) << std::endl;
}


