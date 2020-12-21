//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_PRESSURESENSOR_H
#define MASTERTHESISAPP_PRESSURESENSOR_H

#include "smbus.h"
#include "I2CWrapper.h"
#include <cstdint>
#include <bitset>
class PressureSensor {
public:
    PressureSensor();
    void ReadPressure();

private:
    static constexpr std::uint_fast8_t whoIAmReg_{0x0F};
    static constexpr std::uint8_t ctrlReg1_{0x20};
    static constexpr std::uint8_t ctrlReg2_{0x21};
    static constexpr std::uint8_t statusReg_{0x27};
    static constexpr std::uint8_t pressureLowReg_{0x28};
    static constexpr std::uint8_t pressureMidReg_{0x29};
    static constexpr std::uint8_t pressureHighReg_{0x2A};
    static constexpr std::uint8_t oneMeasureCmd_ = 0;
    static constexpr std::uint8_t deviceOnCmd_ = 7;

    smbus_info_t* PressureCommunicationInfo_ = new smbus_info_t;
    std::bitset<24> rawDataFromSensor_;
    unsigned int dataFromSensor_;

    esp_err_t WriteByte(std::uint8_t reg, std::bitset<8> data);
    esp_err_t ReadByte(std::uint8_t reg, std::bitset<8>& data) const;
    esp_err_t SetValueInByte(std::uint8_t reg, std::uint8_t position);
    esp_err_t ResetValueInByte(std::uint8_t reg, std::uint8_t position);
    void TurnDeviceOn();
    void TurnDeviceOff();
    void EnableOneMeasure();
    bool isProbeAvailable() const;
    static std::bitset<8> ConvertToBitset(std::uint8_t byte);
    static std::uint8_t ConvertToUint8(std::bitset<8> byte);
    void SaveDataFromSensor(const std::bitset<8>& data);
};

#endif // MASTERTHESISAPP_PRESSURESENSOR_H
