//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_PRESSURESENSOR_H
#define MASTERTHESISAPP_PRESSURESENSOR_H

#include <bitset>
#include <cstdint>
#include "I2CWrapper.h"
#include "smbus.h"

class PressureSensor {
public:
    PressureSensor();
    void EnableOneMeasure();
    void PerformReadOut();
    auto GetRawPressure() const -> const std::bitset<24>& {
        return rawPressureData_;
    }
    auto GetPressure() const -> const unsigned int& {
        return pressureData_;
    }
    auto GetRawTemperature() const -> const std::bitset<16>& {
        return rawTempData_;
    }

    auto GetTemperature() const -> const double& {
        return tempData_;
    }

private:
    static constexpr std::uint_fast8_t whoIAmReg_{0x0F};
    static constexpr std::uint8_t ctrlReg1_{0x20};
    static constexpr std::uint8_t ctrlReg2_{0x21};
    static constexpr std::uint8_t statusReg_{0x27};
    static constexpr std::uint8_t pressureLowReg_{0x28};
    static constexpr std::uint8_t pressureMidReg_{0x29};
    static constexpr std::uint8_t pressureHighReg_{0x2A};
    static constexpr std::uint8_t tempLowReg_{0x2B};
    static constexpr std::uint8_t tempHighReg_{0x2C};
    static constexpr std::uint8_t oneMeasureIndex = 0;

    static constexpr std::uint8_t turnOnIndex = 7;
    smbus_info_t* PressureCommunicationInfo_ = new smbus_info_t;
    std::bitset<24> rawPressureData_;
    unsigned int pressureData_ = 0;
    std::bitset<16> rawTempData_;

    double tempData_ = 0;
    void TurnDeviceOn();
    void TurnDeviceOff();
    void ReadPressure();
    void ReadTemperature();
    bool isProbeAvailable() const;

    template<std::size_t B>
    void SaveDataFromSensor(const std::bitset<8>& dataFrom,
                            std::bitset<B>& dataTo);
    esp_err_t ReadByte(std::uint8_t reg, std::bitset<8>& data) const;
    esp_err_t SetValueInByte(std::uint8_t reg, std::uint8_t position);
    esp_err_t ResetValueInByte(std::uint8_t reg, std::uint8_t position);
    esp_err_t WriteByte(std::uint8_t reg, std::bitset<8> data);
    static std::bitset<8> ConvertToBitset(std::uint8_t byte);
    static std::uint8_t ConvertToUint8(std::bitset<8> byte);
    template<std::size_t B>
    static long ConvertToLong(const std::bitset<B>& b);
};

#endif // MASTERTHESISAPP_PRESSURESENSOR_H
