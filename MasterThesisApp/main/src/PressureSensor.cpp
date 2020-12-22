//
// Created by lukaszk on 19.12.2020.
//

#include <bitset>
#include <iostream>
#include "include/PressureSensor.h"
#include "smbus.h"
#include "include/Definitions.h"
#include "esp_log.h"

PressureSensor::PressureSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    PressureCommunicationInfo_ = i2cWrapper.GetsmBusInfoPressure_();
    TurnDeviceOn();
    EnableOneMeasure();
}

void PressureSensor::EnableOneMeasure() {
    std::array<std::uint8_t, 1> positions{oneMeasureIndex};
    SetValuesInByte(ctrlReg2_, positions);
    std::bitset<8> byte;
    ReadByte(ctrlReg2_, byte);
    if (!byte.test(oneMeasureIndex)) {
        ESP_LOGE(devicePressSens, "Measuring off: reg: %lu", byte.to_ulong());
    }
}
void PressureSensor::PerformReadOut() {
    if (isProbeAvailable()) {
        ReadPressure();
        ESP_LOGI(devicePressSens, "Pressure is: %du", pressureData_);
        ReadTemperature();
        ESP_LOGI(devicePressSens, "Temp is: %f", tempData_);
    }
}
void PressureSensor::TurnDeviceOn() {
    std::array<std::uint8_t, 1> positions{turnOnIndex};
    SetValuesInByte(ctrlReg1_, positions);
    std::bitset<8> byte;
    ReadByte(ctrlReg1_, byte);
    if (!byte.test(turnOnIndex)) {
        ESP_LOGE(devicePressSens, "Device off, reg: %lu", byte.to_ulong());
    }
}
void PressureSensor::TurnDeviceOff() {
    std::array<std::uint8_t, 1> positions{turnOnIndex};
    ResetValuesInByte(ctrlReg1_, positions);
}

void PressureSensor::ReadPressure() {
    std::bitset<8> data;
    ReadByte(pressureHighReg_, data);
    SaveDataFromSensor(data, rawPressureData_);
    rawPressureData_ = rawPressureData_ << 8;
    ReadByte(pressureMidReg_, data);
    SaveDataFromSensor(data, rawPressureData_);
    rawPressureData_ = rawPressureData_ << 8;
    ReadByte(pressureLowReg_, data);
    SaveDataFromSensor(data, rawPressureData_);
    pressureData_ = rawPressureData_.to_ulong() / 4096;
}
void PressureSensor::ReadTemperature() {
    std::bitset<8> data;
    ReadByte(tempHighReg_, data);
    SaveDataFromSensor(data, rawTempData_);
    rawTempData_ = rawTempData_ << 8;
    ReadByte(tempLowReg_, data);
    SaveDataFromSensor(data, rawTempData_);
    tempData_ = 42.5 + ConvertToLong(rawTempData_) / 480.0;
}
bool PressureSensor::isProbeAvailable() const {
    std::bitset<8> data;
    std::size_t pressureBitIndex = 1;
    ReadByte(statusReg_, data);
    std::cout << "measure ready? 000010: " << data << std::endl;
    return data.test(pressureBitIndex);
}
template<std::size_t To, std::size_t From>
void PressureSensor::SaveDataFromSensor(const std::bitset<From>& dataFrom,
                                        std::bitset<To>& dataTo) {
    if(From > To){
        ESP_LOGE(devicePressSens, "Too much data, %d > %d", From, To);
    }
    else{
        for (std::size_t i = 0; i < From; i++) {
            dataTo[i] = dataFrom[i];
        }
    }
}
esp_err_t PressureSensor::ReadByte(std::uint8_t reg,
                                   std::bitset<8>& data) const {
    esp_err_t err;
    std::uint8_t byte = ConvertToUint8(data);
    err = smbus_read_byte(PressureCommunicationInfo_, reg, &byte);
    data = ConvertToBitset(byte);
    return err;
}
template <std::size_t B>
esp_err_t PressureSensor::SetValuesInByte(std::uint8_t reg,
                                         std::array<std::uint8_t, B> positions) {
    std::bitset<8> valueInRegister;
    ReadByte(reg, valueInRegister);
    for (const auto &val : positions){
        valueInRegister.set(static_cast<int>(val));
    }
    return WriteByte(reg, valueInRegister);
}
template<std::size_t B>
esp_err_t PressureSensor::ResetValuesInByte(std::uint8_t reg,
                                            std::array<std::uint8_t, B> positions) {
    std::bitset<8> valueInRegister;
    ReadByte(reg, valueInRegister);
    for (const auto &val : positions){
        valueInRegister.reset(static_cast<int>(val));
    }
    return WriteByte(reg, valueInRegister);
}
esp_err_t PressureSensor::WriteByte(std::uint8_t reg, std::bitset<8> data) {
    return smbus_write_byte(PressureCommunicationInfo_,
                            reg,
                            ConvertToUint8(data));
}
std::bitset<8> PressureSensor::ConvertToBitset(std::uint8_t byte) {
    return static_cast<std::bitset<8>>(byte);
}
std::uint8_t PressureSensor::ConvertToUint8(std::bitset<8> byte) {
    return static_cast<uint8_t>(byte.to_ulong());
}
template<std::size_t B>
long PressureSensor::ConvertToLong(const std::bitset<B>& b) {
    //© 1997-2005 Bit Twiddling Hacks Sean Eron Anderson
    struct {
        long x : B;
    } s;
    return s.x = b.to_ulong();
}
