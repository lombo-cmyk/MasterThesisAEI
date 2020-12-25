//
// Created by lukaszk on 22.12.2020.
//

#include "include/ParticlesSensor.h"
#include "include/I2CWrapper.h"
#include <iostream>
#include "include/Definitions.h"
#include "esp_log.h"
#include "bitset"
#include <memory>
#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK true
#define NO_ACK_CHECK false
#define ACK_VALUE 0x0
#define NACK_VALUE 0x1

ParticlesSensor::ParticlesSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    ParticlesCommunicationInfo_ = i2cWrapper.GetsmBusInfoPm();
    StartMeasuring(false);
}
std::uint8_t ParticlesSensor::CalculateCrc(std::uint8_t dataLSB,
                                           std::uint8_t dataMSB) {
    /// Datasheet Sensirion SPS30
    /// articulate Matter Sensor for Air Quality Monitoring and Control
    std::array<std::uint8_t, 2> data = {dataLSB, dataMSB};
    uint8_t crc = 0xFFu;
    for (int i = 0; i < 2; i++) {
        crc ^= data[i];
        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80u) {
                crc = (crc << 1u) ^ 0x31u; // NOLINT Wrong type assumed
            } else {
                crc = (crc << 1u);
            }
        }
    }
    return crc;
}
void ParticlesSensor::PerformReadout() {
    std::array<std::uint8_t, 30> data{};
    if (isParticleDataAvailable()) {
        SetPointerAndRead(readMeasureDataPtr_, data);
        if (IsCrcInDataValid(data)) {
            ConvertReadData(data);
        }
    }
}
template<std::size_t B>
bool ParticlesSensor::IsCrcInDataValid(
    const std::array<std::uint8_t, B>& data) const {
    bool valid = true;
    for (std::size_t i = 0; i < data.size(); i += 3) {
        std::size_t crc = CalculateCrc(data[i], data[i + 1]);
        if (crc != data[i + 2]) {
            ESP_LOGE(devicePmSens,
                     "CRC fail in dataread, data: %02x, calculated: %02x",
                     data[i + 2],
                     crc);
            ESP_LOGE(devicePmSens, "Data corrupted, breaking");
            valid = false;
            break;
        }
    }
    return valid;
}
template<std::size_t B>
void ParticlesSensor::ConvertReadData(
    const std::array<std::uint8_t, B>& data) {
    // todo: enable calculating flot basing on data.size() or outputFormatByte
    PM25 = static_cast<std::uint16_t>((data[3] << 8u) + data[4]);
    PM10 = static_cast<std::uint16_t>((data[9] << 8u) + data[10]);
}
void ParticlesSensor::StartMeasuring(bool measureFloat) {
    if (measureFloat) {
        outputFormatByte = 0x03;
    } else {
        outputFormatByte = 0x05;
    }
    std::array<std::uint8_t, 2> data{outputFormatByte, 0x00};
    SetPointerAndWrite(measureStartPtr_, data);
}
void ParticlesSensor::StopMeasuring() const {
    SetPointer(measureStopPtr_);
}
template<std::size_t B>
void ParticlesSensor::SetPointerAndRead(
    const std::uint16_t ptrAddress,
    std::array<std::uint8_t, B>& data) const {
    SetPointer(ptrAddress);
    ReadData(data);
}
template<std::size_t B>
void ParticlesSensor::SetPointerAndWrite(
    const std::uint16_t ptrAddress,
    std::array<std::uint8_t, B>& data) const {
    esp_err_t error;
    std::array<std::uint8_t, 2> ptrAd = {
        static_cast<uint8_t>(ptrAddress & 0xFFu),
        static_cast<uint8_t>((ptrAddress >> 8u) & 0xFFu)}; // NOLINT
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 | // NOLINT
                              WRITE_BIT,
                          ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[1], ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[0], ACK_CHECK);
    for (std::size_t i = 0; i < data.size(); i += 2) {
        i2c_master_write_byte(commandHandle, data[i], ACK_CHECK);
        i2c_master_write_byte(commandHandle, data[i + 1], ACK_CHECK);
        i2c_master_write_byte(commandHandle,
                              CalculateCrc(data[i], data[i + 1]),
                              ACK_CHECK);
    }
    i2c_master_stop(commandHandle);
    error = i2c_master_cmd_begin(ParticlesCommunicationInfo_->i2c_port,
                                 commandHandle,
                                 ParticlesCommunicationInfo_->timeout);
    std::cout << "Error set write: " << error << std::endl;
    i2c_cmd_link_delete(commandHandle);
}
void ParticlesSensor::SetPointer(const std::uint16_t ptrAddress) const {
    esp_err_t error;
    std::array<std::uint8_t, 2> ptrAd = {
        static_cast<uint8_t>(ptrAddress & 0xFFu),
        static_cast<uint8_t>((ptrAddress >> 8u) & 0xFFu)}; // NOLINT
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 | // NOLINT
                              WRITE_BIT,
                          ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[1], ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[0], ACK_CHECK);
    i2c_master_stop(commandHandle);
    error = i2c_master_cmd_begin(ParticlesCommunicationInfo_->i2c_port,
                                 commandHandle,
                                 ParticlesCommunicationInfo_->timeout);
    std::cout << "Error set pointer: " << error << std::endl;
    i2c_cmd_link_delete(commandHandle);
}
template<std::size_t B>
void ParticlesSensor::ReadData(std::array<std::uint8_t, B>& data) const {
    esp_err_t err;
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 | READ_BIT, // NOLINT
                          ACK_CHECK);
    i2c_master_read(commandHandle,
                    data.data(),
                    data.size(),
                    I2C_MASTER_LAST_NACK);
    i2c_master_stop(commandHandle);
    err = i2c_master_cmd_begin(ParticlesCommunicationInfo_->i2c_port,
                               commandHandle,
                               ParticlesCommunicationInfo_->timeout);
    std::cout << "Error_1: " << err << std::endl;
    i2c_cmd_link_delete(commandHandle);
}
bool ParticlesSensor::isParticleDataAvailable() const {
    std::array<std::uint8_t, 3> data{};
    SetPointerAndRead(DataReadyFlagPtr_, data);
    return static_cast<bool>(data[1]);
}
