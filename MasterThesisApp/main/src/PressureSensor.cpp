//
// Created by lukaszk on 19.12.2020.
//

#include "include/PressureSensor.h"
#include "smbus.h"
#include <iostream>
#include <bitset>
#include "include/Definitions.h"
#include "esp_log.h"

PressureSensor::PressureSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    PressureCommunicationInfo_ = i2cWrapper.GetsmBusInfoPressure_();
    TurnDeviceOn();
    EnableOneMeasure();
}

void PressureSensor::TurnDeviceOn() {
    SetValueInByte(ctrlReg1_, deviceOnCmd_);
    std::bitset<8> a;
    ReadByte(ctrlReg1_, a);
    std::cout << "Device oN? 100000: " << a << std::endl;
}

void PressureSensor::TurnDeviceOff() {
    ResetValueInByte(ctrlReg1_, deviceOnCmd_);
}
void PressureSensor::EnableOneMeasure() {
    SetValueInByte(ctrlReg2_, oneMeasureCmd_);
    std::bitset<8> a;
    ReadByte(ctrlReg2_, a);
    std::cout << "measure on? 000001: " << a << std::endl;
}
void PressureSensor::ReadPressure() {
    esp_err_t err;
    if (isProbeAvailable()) {
        std::bitset<8> data;
        ReadByte(pressureHighReg_, data);
        SaveDataFromSensor(data);
        rawDataFromSensor_ = rawDataFromSensor_ << 8;
        ReadByte(pressureMidReg_, data);
        SaveDataFromSensor(data);
        rawDataFromSensor_ = rawDataFromSensor_ << 8;
        ReadByte(pressureLowReg_, data);
        SaveDataFromSensor(data);
        dataFromSensor_ = rawDataFromSensor_.to_ulong() / 4096;
        ESP_LOGI(devicePressSens,
                 "Pressure is: %lu",
                 rawDataFromSensor_.to_ulong() / 4096);
    }
}

void PressureSensor::SaveDataFromSensor(const std::bitset<8>& data) {
    for (std::uint8_t i = 0; i < 8; i++) {
        rawDataFromSensor_[i] = data[i];
    }
}
bool PressureSensor::isProbeAvailable() const {
    std::bitset<8> data;
    std::uint8_t pressureBitIndex = 1;
    ReadByte(statusReg_, data);
    std::cout << "measure ready? 000010: " << data << std::endl;
    return data.test(pressureBitIndex);
}
esp_err_t PressureSensor::SetValueInByte(std::uint8_t reg,
                                         std::uint8_t position) {
    std::bitset<8> valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister.set(static_cast<int>(position));
    return WriteByte(reg, valueInRegister);
}
esp_err_t PressureSensor::ResetValueInByte(std::uint8_t reg,
                                           std::uint8_t position) {
    std::bitset<8> valueInRegister;
    ReadByte(reg, valueInRegister);
    valueInRegister.reset(static_cast<int>(position));
    return WriteByte(reg, valueInRegister);
}
esp_err_t PressureSensor::ReadByte(std::uint8_t reg,
                                   std::bitset<8>& data) const {
    esp_err_t err;
    std::uint8_t byte = ConvertToUint8(data);
    err = smbus_read_byte(PressureCommunicationInfo_, reg, &byte);
    data = ConvertToBitset(byte);
    return err;
}
esp_err_t PressureSensor::WriteByte(std::uint8_t reg, std::bitset<8> data) {
    return smbus_write_byte(PressureCommunicationInfo_,
                            reg,
                            ConvertToUint8(data));
}
std::bitset<8> PressureSensor::ConvertToBitset(std::uint8_t byte) {
    return static_cast<std::bitset<8>>(byte);
    ;
}
std::uint8_t PressureSensor::ConvertToUint8(std::bitset<8> byte) {
    return static_cast<uint8_t>(byte.to_ulong());
}
