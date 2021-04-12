//
// Created by lukaszk on 22.12.2020.
//

#ifndef MASTERTHESISAPP_PARTICLESSENSOR_H
#define MASTERTHESISAPP_PARTICLESSENSOR_H
#include "smbus.h"
#include <bitset>
#include <array>
#include <memory>
#include <tuple>
#include "I2Ccommon.h"

class ParticlesSensor : private I2Ccommon {
public:
    ParticlesSensor();
    bool StartMeasuring(bool measureFloat);
    bool StopMeasuring();
    bool PerformReadout();

    auto GetPM25() const -> const std::uint16_t& {
        return PM25;
    }
    auto GetPM10() const -> const std::uint16_t& {
        return PM10;
    }

private:
    smbus_info_t* ParticlesCommunicationInfo_;
    esp_err_t previousRealError_ = 0, currentError_ = 0;
    std::uint16_t PM25 = 0, PM10 = 0;
    std::uint16_t measureStartPtr_ = 0x0010;
    std::uint16_t measureStopPtr_ = 0x0104;
    std::uint16_t readMeasureDataPtr_ = 0x0300;
    std::uint16_t DataReadyFlagPtr_ = 0x0202;
    std::uint8_t outputFormatByte = 0x05;

    bool IsParticleDataAvailable();
    template<std::size_t B>
    std::pair<esp_err_t, esp_err_t>
    SetPointerAndRead(std::uint16_t ptrAddress,
                      std::array<std::uint8_t, B>& data) const;
    template<std::size_t B>
    esp_err_t SetPointerAndWrite(std::uint16_t ptrAddress,
                                 std::array<uint8_t, B>& data) const;
    esp_err_t SetPointer(std::uint16_t ptrAddress) const;
    template<std::size_t B>
    esp_err_t ReadData(std::array<std::uint8_t, B>& data) const;
    template<std::size_t B>
    void ConvertReadData(const std::array<std::uint8_t, B>& data);
    static std::uint8_t CalculateCrc(std::uint8_t data_0, std::uint8_t data_1);
    template<std::size_t B>
    bool IsCrcInDataValid(const std::array<std::uint8_t, B>& data) const;

    void UpdateModbusRegisters() const;
};

#endif // MASTERTHESISAPP_PARTICLESSENSOR_H
