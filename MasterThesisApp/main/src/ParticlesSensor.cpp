//
// Created by lukaszk on 22.12.2020.
//

#include "include/ParticlesSensor.h"
#include "include/I2CWrapper.h"
#include "driver/i2c.h"
#include <iostream>
#include "include/Definitions.h"
#include "bitset"
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
std::uint8_t ParticlesSensor::CalculateCrc(std::uint8_t data_0,
                                           std::uint8_t data_1) {
    std::uint8_t data[2] = {data_0, data_1};
    uint8_t crc = 0xFF;
    for (int i = 0; i < 2; i++) {
        crc ^= data[i];
        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31u;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}
void ParticlesSensor::PerformReadOut() {
    std::uint8_t data[30];
    if (isParticleDataAvailable()) {
        SetPointerAndRead(readMeasureDataPtr_, data, 30);
    }
    for (std::size_t i=0; i<30; i+=3){
        std::size_t crc=CalculateCrc(data[i], data[i+1]);
        if(crc == static_cast<std::size_t>(data[i+2])){
            std::uint16_t measure = (data[i] << 8) + data[i+1];
            std::cout << "Measure: " << static_cast<int>(measure) <<std::endl;
            if (i==3){
                PM25 = static_cast<double>(measure);
            }
            if (i==9){
                PM10 =static_cast<double>(measure);
            }
        }
        else{
            std::cout << "CRC FAIL" <<std::endl;
        }
    }
}
void ParticlesSensor::StartMeasuring(bool measureFloat) {
    std::uint8_t outputFormatByte;
    if (measureFloat) {
        outputFormatByte = 0x03;
    } else {
        outputFormatByte = 0x05;
    }
    std::uint8_t data[2] = {outputFormatByte, 0x00};
    SetPointerAndWrite(measureStartPtr_, data, 2);
}
void ParticlesSensor::StopMeasuring() {}
void ParticlesSensor::SetPointerAndRead(const std::uint16_t ptrAddress,
                                        std::uint8_t* data,
                                        std::size_t len) {
    SetPointer(ptrAddress);
    ReadData(data, len);
}
void ParticlesSensor::SetPointerAndWrite(const std::uint16_t ptrAddress,
                                         uint8_t* data,
                                         uint8_t len) {
    esp_err_t error;
    uint8_t ptrAd[2] = {static_cast<uint8_t>(ptrAddress & 0xffu),
                        static_cast<uint8_t>((ptrAddress >> 8u) & 0xffu)};
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 |
                              WRITE_BIT,
                          ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[1], ACK_CHECK);
    i2c_master_write_byte(commandHandle, ptrAd[0], ACK_CHECK);
    for (std::size_t i = 0; i < len; i += 2) {
        i2c_master_write_byte(commandHandle, data[i], ACK_CHECK);
        i2c_master_write_byte(commandHandle, ptrAd[i + 1], ACK_CHECK);
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
void ParticlesSensor::SetPointer(const std::uint16_t ptrAddress) {
    esp_err_t error;
    uint8_t ptrAd[2] = {static_cast<uint8_t>(ptrAddress & 0xffu),
                        static_cast<uint8_t>((ptrAddress >> 8u) & 0xffu)};
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 |
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
void ParticlesSensor::ReadData(std::uint8_t* data, std::size_t len) {
    esp_err_t err;
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 | READ_BIT,
                          ACK_CHECK);
    i2c_master_read(commandHandle, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(commandHandle);
    err = i2c_master_cmd_begin(ParticlesCommunicationInfo_->i2c_port,
                               commandHandle,
                               ParticlesCommunicationInfo_->timeout);
    std::cout << "Error_1: " << err << std::endl;
    i2c_cmd_link_delete(commandHandle);
}
bool ParticlesSensor::isParticleDataAvailable() {
    std::uint8_t data[3];
    SetPointerAndRead(DataReadyFlagPtr_, data, 3);
    return static_cast<bool>(data[1]);
}
