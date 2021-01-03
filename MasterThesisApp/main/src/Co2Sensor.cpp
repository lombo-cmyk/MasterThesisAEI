//
// Created by lukaszk on 02.01.2021.
//

#include "include/Co2Sensor.h"
#include "include/I2CWrapper.h"
#include "smbus.h"
#include <iostream>
#include "include/Converter.h"
#include "include/Definitions.h"

Co2Sensor::Co2Sensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    Co2CommunicationInfo_ = i2cWrapper.GetsmBusInfoCo2();
}
bool Co2Sensor::StartMeasuring() {
    std::uint8_t startCommand = 0x06;
    esp_err_t error = WriteByte(operatingModePtr_, startCommand);
    return IsErrorInCommunication(error, deviceCo2Sens);
}
bool Co2Sensor::StopMeasuring() {
    std::uint8_t stopCommand = 0x00;
    esp_err_t error = WriteByte(operatingModePtr_, stopCommand);
    return IsErrorInCommunication(error, deviceCo2Sens);
}
bool Co2Sensor::PerformReadout() {
    bool isNewData = false;
    std::array<std::uint8_t, 2> data{};
    std::pair<esp_err_t, esp_err_t> errors;
    errors = ReadCo2(data);
    if (!IsErrorInCommunication(std::get<0>(errors), deviceCo2Sens) &&
        !IsErrorInCommunication(std::get<1>(errors), deviceCo2Sens)) {
        isNewData = true;
        Co2Value_ = (data[1] << 8u) + data[0];
        ESP_LOGI(deviceCo2Sens, "Co2 readout: %d", Co2Value_);
    }
    return isNewData;
}
bool Co2Sensor::IsDeviceOn() {
    return false;
}
esp_err_t Co2Sensor::WriteByte(std::uint8_t ptr, std::uint8_t data) {
    esp_err_t error = ESP_DEVICE_BUSY;
    if (IsSensorFree()) {
        error = smbus_write_byte(Co2CommunicationInfo_, ptr, data);
    }
    return error;
}
std::pair<esp_err_t, esp_err_t>
Co2Sensor::ReadCo2(std::array<std::uint8_t, 2>& data) {
    esp_err_t lowByteError = ESP_DEVICE_BUSY, highByteError = ESP_DEVICE_BUSY;
    if (IsSensorFree()) {
        lowByteError = smbus_read_byte(Co2CommunicationInfo_,
                                       lowCo2DataPtr_,
                                       &data[0]);
        highByteError = smbus_read_byte(Co2CommunicationInfo_,
                                        highCo2DataPtr_,
                                        &data[1]);
    }
    return std::make_pair(lowByteError, highByteError);
}
bool Co2Sensor::IsSensorFree() {
    return !IsSensorBusyAnalog() && !IsSensorBusyI2C();
}
bool Co2Sensor::IsSensorBusyI2C() {
    std::uint8_t data;
    smbus_read_byte(Co2CommunicationInfo_, readStatusPtr_, &data);
    std::bitset<8> a = ConvertToBitset(data);
    return static_cast<bool>(a[7]);
}
bool Co2Sensor::IsSensorBusyAnalog() {
    return gpio_get_level(CO2_BUSY_BUTTON);
}
