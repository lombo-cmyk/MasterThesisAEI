//
// Created by lukaszk on 19.12.2020.
//

#include "include/PressureSensor.h"
#include "smbus.h"
#include <iostream>
#include <bitset>
#include "include/Definitions.h"
PressureSensor::PressureSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    PressureCommunicationInfo_ = i2cWrapper.GetsmBusInfoPressure_();
}

void PressureSensor::ReadPressure() {
    esp_err_t err;
    err = ReadByte(whoIAmReg_, dataFromSensor_);
    std::cout << "Error: " << err << std::endl;
    std::cout << "Dummy register data is: "
              << static_cast<std::bitset<8>>(dataFromSensor_) << std::endl;
}

esp_err_t PressureSensor::ReadByte(std::uint8_t reg, uint8_t& data) {
    return smbus_read_byte(PressureCommunicationInfo_, reg, &data);
}
esp_err_t PressureSensor::WriteByte(std::uint8_t reg,
                                    std::uint8_t data) {
    return smbus_write_byte(PressureCommunicationInfo_, reg, data);
}
esp_err_t PressureSensor::SetValuesInByte(std::uint8_t reg, uint8_t byte) {
    std::uint8_t valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister |= byte;
    return WriteByte(reg, valueInRegister);
}
esp_err_t  PressureSensor::ResetValuesInByte(std::uint8_t reg, uint8_t byte) {
    std::uint8_t valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister &= ~byte;
    return WriteByte(reg, valueInRegister);
}
