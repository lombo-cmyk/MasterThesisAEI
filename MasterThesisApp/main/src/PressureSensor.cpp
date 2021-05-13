//
// Created by lukaszk on 19.12.2020.
//

#include <bitset>
#include "PressureSensor.h"
#include "smbus.h"
#include "Definitions.h"
#include "esp_log.h"
#include "Converter.h"
#include "ModbusDefinitions.h"
#include "Modbus.h"

PressureSensor::PressureSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    PressureCommunicationInfo_ = i2cWrapper.GetsmBusInfoPressure_();
    //    TurnDeviceOn();
    //    EnableOneMeasure();
}

bool PressureSensor::TurnDeviceOn() {
    std::array<std::uint8_t, 1> positions{turnOnIndex};
    bool ret = IsErrorInCommunication(SetValuesInByte(ctrlReg1_, positions),
                                      devicePressSens);
    // todo: move below to public IsDeviceOn();
    std::bitset<8> byte;
    ReadBytes(ctrlReg1_, byte);
    if (!byte.test(turnOnIndex)) {
        ESP_LOGE(devicePressSens, "Device off, reg: %lu", byte.to_ulong());
    }
    return ret;
}

bool PressureSensor::TurnDeviceOff() {
    // todo: implement public IsDeviceOff();
    std::array<std::uint8_t, 1> positions{turnOnIndex};
    return IsErrorInCommunication(ResetValuesInByte(ctrlReg1_, positions),
                                  devicePressSens);
}

bool PressureSensor::EnableOneMeasure() {
    std::array<std::uint8_t, 1> positions{oneMeasureIndex};
    bool ret = IsErrorInCommunication(SetValuesInByte(ctrlReg2_, positions),
                                      devicePressSens);
    // todo: move below to public IsOneMeasureEnabled();
    std::bitset<8> byte;
    ReadBytes(ctrlReg2_, byte);
    if (!byte.test(oneMeasureIndex)) {
        ESP_LOGE(devicePressSens, "Measuring off: reg: %lu", byte.to_ulong());
    }
    return ret;
}

bool PressureSensor::PerformReadOut() {
    bool ret = false;
    if (isProbeAvailable()) {
        esp_err_t errorPressure = ReadPressure();
        esp_err_t errorTemp = ReadTemperature();
        ret = !(IsErrorInCommunication(errorPressure, devicePressSens) ||
                IsErrorInCommunication(errorTemp, devicePressSens));
        ESP_LOGI(devicePressSens, "Pressure is: %du", pressureData_);
        ESP_LOGI(devicePressSens, "Temp is: %f", tempData_);
        UpdateModbusRegisters();
    }
    return ret;
}

esp_err_t PressureSensor::ReadPressure() {
    esp_err_t error;
    const std::size_t pressureBytes = 3;
    std::bitset<8 * pressureBytes> data;
    error = ReadBytes(pressureLowReg_, data);
    if (!error) {
        SaveDataFromSensor(data, rawPressureData_);
        pressureData_ = rawPressureData_.to_ulong() / 4096;
    }
    return error;
}

esp_err_t PressureSensor::ReadTemperature() {
    esp_err_t error;
    const std::size_t temperatureBytes = 2;
    std::bitset<8 * temperatureBytes> data;
    error = ReadBytes(tempLowReg_, data);
    if (!error) {
        SaveDataFromSensor(data, rawTempData_);
        tempData_ = 42.5 + ConvertToLong(rawTempData_) / 480.0;
    }
    return error;
}

bool PressureSensor::isProbeAvailable() {
    bool isProbe = false;
    esp_err_t error;
    std::bitset<8> data{};
    std::size_t pressureBitIndex = 1;
    error = ReadBytes(statusReg_, data);
    if (!IsErrorInCommunication(error, devicePressSens)) {
        isProbe = static_cast<bool>(data[pressureBitIndex]);
    }
    return isProbe;
}

template<std::size_t To, std::size_t From>
void PressureSensor::SaveDataFromSensor(const std::bitset<From>& dataFrom,
                                        std::bitset<To>& dataTo) {
    if (From > To) {
        ESP_LOGE(devicePressSens, "Too much data, %d > %d", From, To);
    } else {
        for (std::size_t i = 0; i < From; i++) {
            dataTo[i] = dataFrom[i];
        }
    }
}

template<std::size_t B>
esp_err_t PressureSensor::ReadBytes(std::uint8_t reg,
                                    std::bitset<B>& data) const {
    esp_err_t err;
    const std::size_t noBytes = B / 8;
    std::array<std::uint8_t, noBytes> bytes{};
    if (noBytes > 1) {
        reg |= 0b10000000u;
    }
    err = smbus_i2c_read_block(PressureCommunicationInfo_,
                               reg,
                               bytes.data(),
                               bytes.size());
    for (std::size_t i = bytes.size() - 1; i > 0; i--) {
        data |= bytes[i] << (8 * i);
    }
    data |= bytes[0];
    return err;
}

template<std::size_t B>
esp_err_t
PressureSensor::SetValuesInByte(std::uint8_t reg,
                                std::array<std::uint8_t, B> positions) {
    std::bitset<8> valueInRegister;
    ReadBytes(reg, valueInRegister);
    for (const auto& val : positions) {
        valueInRegister.set(static_cast<int>(val));
    }
    return WriteByte(reg, valueInRegister);
}

template<std::size_t B>
esp_err_t
PressureSensor::ResetValuesInByte(std::uint8_t reg,
                                  std::array<std::uint8_t, B> positions) {
    std::bitset<8> valueInRegister;
    ReadBytes(reg, valueInRegister);
    for (const auto& val : positions) {
        valueInRegister.reset(static_cast<int>(val));
    }
    return WriteByte(reg, valueInRegister);
}

esp_err_t PressureSensor::WriteByte(const std::uint8_t reg,
                                    const std::bitset<8> data) {
    std::uint8_t d = ConvertToUint8(data);
    return smbus_i2c_write_block(PressureCommunicationInfo_, reg, &d, 1);
}
void PressureSensor::UpdateModbusRegisters() const {
    auto& modbusManager = Modbus::getInstance();
    std::array<std::uint8_t , 2> index{indexTemperature, indexPressure};
    std::array<float , 2> values{static_cast<float>(tempData_),
                                 static_cast<float>(pressureData_)};
    modbusManager.UpdateHoldingRegs(index, values);
}
