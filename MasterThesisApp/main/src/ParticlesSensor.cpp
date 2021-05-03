//
// Created by lukaszk on 22.12.2020.
//

#include "ParticlesSensor.h"
#include "I2CWrapper.h"
#include "Definitions.h"
#include "ModbusDefinitions.h"
#include "Modbus.h"
#include "esp_log.h"
#include "bitset"
#include <tuple>
// todo: implement fan cleaning function
ParticlesSensor::ParticlesSensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    ParticlesCommunicationInfo_ = i2cWrapper.GetsmBusInfoPm();
}
bool ParticlesSensor::StartMeasuring(bool measureFloat) {
    if (measureFloat) {
        outputFormatByte = 0x03;
    } else {
        outputFormatByte = 0x05;
    }
    std::array<std::uint8_t, 2> data{outputFormatByte, 0x00};
    return IsErrorInCommunication(SetPointerAndWrite(measureStartPtr_, data),
                                  devicePmSens);
}
bool ParticlesSensor::StopMeasuring() {
    return IsErrorInCommunication(SetPointer(measureStopPtr_), devicePmSens);
}
bool ParticlesSensor::PerformReadout() {
    std::array<std::uint8_t, 30> data{};
    std::pair<esp_err_t, esp_err_t> error;
    bool isNewData = false;
    if (IsParticleDataAvailable()) {
        error = SetPointerAndRead(readMeasureDataPtr_, data);
        if (!IsErrorInCommunication(std::get<0>(error), devicePmSens) &&
            !IsErrorInCommunication(std::get<1>(error), devicePmSens) &&
            IsCrcInDataValid(data)) {
            ConvertReadData(data);
            isNewData = true;
            UpdateModbusRegisters();
        }
    }
    return isNewData;
}
bool ParticlesSensor::IsParticleDataAvailable() {
    bool isData = false;
    std::array<std::uint8_t, 3> data{};
    std::pair<esp_err_t, esp_err_t> error;
    error = SetPointerAndRead(DataReadyFlagPtr_, data);
    if (!IsErrorInCommunication(std::get<0>(error), devicePmSens) &&
        !IsErrorInCommunication(std::get<1>(error), devicePmSens)) {
        isData = static_cast<bool>(data[1]);
    }
    return isData;
}
template<std::size_t B>
void ParticlesSensor::ConvertReadData(
    const std::array<std::uint8_t, B>& data) {
    // todo: enable calculating flot basing on data.size() or outputFormatByte
    auto pm1 = static_cast<std::uint16_t>((data[0] << 8u) + data[1]);
    PM25 = static_cast<std::uint16_t>((data[3] << 8u) + data[4]);
    PM10 = static_cast<std::uint16_t>((data[9] << 8u) + data[10]);
    auto par_siz = static_cast<std::uint16_t>((data[27] << 8u) + data[28]);
    ESP_LOGI(devicePmSens, "PM1 is: %d", pm1);
    ESP_LOGI(devicePmSens, "PM2.5 is: %d", PM25);
    ESP_LOGI(devicePmSens, "PM10 is: %d", PM10);
    ESP_LOGI(devicePmSens, "particle size is: %d", par_siz);
}
template<std::size_t B>
std::pair<esp_err_t, esp_err_t>
ParticlesSensor::SetPointerAndRead(const std::uint16_t ptrAddress,
                                   std::array<std::uint8_t, B>& data) const {
    esp_err_t errorSet, errorRead;
    errorSet = SetPointer(ptrAddress);
    errorRead = ReadData(data);
    return std::make_pair(errorSet, errorRead);
}
template<std::size_t B>
esp_err_t
ParticlesSensor::SetPointerAndWrite(const std::uint16_t ptrAddress,
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
    i2c_cmd_link_delete(commandHandle);
    return error;
}
esp_err_t ParticlesSensor::SetPointer(const std::uint16_t ptrAddress) const {
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
    i2c_cmd_link_delete(commandHandle);
    return error;
}
template<std::size_t B>
esp_err_t ParticlesSensor::ReadData(std::array<std::uint8_t, B>& data) const {
    esp_err_t error;
    i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
    i2c_master_start(commandHandle);
    i2c_master_write_byte(commandHandle,
                          ParticlesCommunicationInfo_->address << 1 | // NOLINT
                              READ_BIT,
                          ACK_CHECK);
    i2c_master_read(commandHandle,
                    data.data(),
                    data.size(),
                    I2C_MASTER_LAST_NACK);
    i2c_master_stop(commandHandle);
    error = i2c_master_cmd_begin(ParticlesCommunicationInfo_->i2c_port,
                                 commandHandle,
                                 ParticlesCommunicationInfo_->timeout);
    i2c_cmd_link_delete(commandHandle);
    return error;
}
std::uint8_t ParticlesSensor::CalculateCrc(const std::uint8_t dataLSB,
                                           const std::uint8_t dataMSB) {
    /// Datasheet Sensirion SPS30
    /// articulate Matter Sensor for Air Quality Monitoring and Control
    std::array<std::uint8_t, 2> data = {dataLSB, dataMSB};
    std::uint8_t crc = 0xFFu;
    for (int i = 0; i < 2; i++) {
        crc ^= data[i];
        for (std::uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80u) {
                crc = (crc << 1u) ^ 0x31u; // NOLINT Wrong type assumed
            } else {
                crc = (crc << 1u);
            }
        }
    }
    return crc;
}
template<std::size_t B>
bool ParticlesSensor::IsCrcInDataValid(
    const std::array<std::uint8_t, B>& data) const {
    bool valid = true;
    for (std::size_t i = 0; i < data.size(); i += 3) {
        std::size_t crc = CalculateCrc(data[i], data[i + 1]);
        if (crc != data[i + 2]) {
            ESP_LOGE(devicePmSens,
                     "CRC fail in data read, data: %02x, calculated: %02x",
                     data[i + 2],
                     crc);
            ESP_LOGE(devicePmSens, "Data corrupted, breaking");
            valid = false;
            break;
        }
    }
    return valid;
}
void ParticlesSensor::UpdateModbusRegisters() const {
    auto& modbusManager = Modbus::getInstance();
    vPortEnterCritical(&modbusMutex);
    holdingRegParams_t regHolding = modbusManager.GetHoldingRegs();
    regHolding[indexPm25] = PM25;
    regHolding[indexPm10] = PM10;
    modbusManager.UpdateHoldingRegs(regHolding);
    vPortExitCritical(&modbusMutex);
}
