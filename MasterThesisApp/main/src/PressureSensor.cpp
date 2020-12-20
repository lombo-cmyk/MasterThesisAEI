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
    TurnDeviceOn();
    StartMeasure();
}

void PressureSensor::TurnDeviceOn() {
    SetValuesInByte(ctrlReg1_, deviceOnCmd_);
    std::uint8_t a;
    ReadByte(ctrlReg1_,a);
    std::cout << "Device oN? 100000: "
              << static_cast<std::bitset<8>>(a) << std::endl;
}

void PressureSensor::TurnDeviceOff() {
    ResetValuesInByte(ctrlReg1_, resetRegisterCmd_);
}
void PressureSensor::StartMeasure() {
    SetValuesInByte(ctrlReg2_, oneMeasureCmd_);
    std::uint8_t a;
    ReadByte(ctrlReg2_,a);
    std::cout << "measure on? 000001: "
              << static_cast<std::bitset<8>>(a) << std::endl;
}
void PressureSensor::ReadPressure() {
    esp_err_t err;
    if(isProbeAvailable()){
//        err = ReadByte(whoIAmReg_, dataFromSensor_);
//        std::cout << "Error: " << err << std::endl;
//        std::cout << "Dummy register data is: "
//            << static_cast<std::bitset<8>>(dataFromSensor_) << std::endl;
    }
}
bool PressureSensor::isProbeAvailable() const{
    std::uint8_t data;
    std::uint8_t pressureBitIndex = 1;
    ReadByte(statusReg_, data);
    std::cout << "measure ready? 000010: "
              << static_cast<std::bitset<8>>(data) << std::endl;
    return isBitSet(data, pressureBitIndex);
}
esp_err_t PressureSensor::SetValuesInByte(std::uint8_t reg, uint8_t byte) {
    std::uint8_t valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister |= byte;
    return WriteByte(reg, valueInRegister);
}
esp_err_t PressureSensor::ResetValuesInByte(std::uint8_t reg, uint8_t byte) {
    std::uint8_t valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister &= ~byte;
    return WriteByte(reg, valueInRegister);
}
esp_err_t PressureSensor::ReadByte(std::uint8_t reg, uint8_t& data) const {
    return smbus_read_byte(PressureCommunicationInfo_, reg, &data);
}
esp_err_t PressureSensor::WriteByte(std::uint8_t reg, std::uint8_t data) {
    return smbus_write_byte(PressureCommunicationInfo_, reg, data);
}
bool PressureSensor::isBitSet(uint8_t byte, uint8_t bitIndex) {
        return ((byte >> bitIndex) & 1) != 0;
}
