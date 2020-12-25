//
// Created by lukaszk on 22.12.2020.
//

#ifndef MASTERTHESISAPP_PARTICLESSENSOR_H
#define MASTERTHESISAPP_PARTICLESSENSOR_H
#include "smbus.h"
#include <bitset>
#include <array>
#include <memory>
class ParticlesSensor {
public:
    ParticlesSensor();
    void PerformReadout();
    auto GetPM25() const -> const std::uint16_t &{
        return PM10;
    }
    auto GetPM10() const -> const std::uint16_t &{
        return PM10;
    }

private:
    std::uint16_t PM25=0, PM10=0;
    smbus_info_t* ParticlesCommunicationInfo_;
    std::uint16_t measureStartPtr_ = 0x0010; // write 3 (including crc)
    std::uint16_t measureStopPtr_ = 0x0104;
    std::uint16_t readMeasureDataPtr_ = 0x0300; // ans float 60 / int 30
    std::uint16_t DataReadyFlagPtr_ = 0x0202; // ans 3 bytes
    std::uint8_t outputFormatByte = 0x05; //0x03 float / 0x05 int

    bool isParticleDataAvailable() const;
    void StartMeasuring(bool measureFloat);
    void StopMeasuring() const;
    template<std::size_t B>
    void SetPointerAndRead(std::uint16_t ptrAddress,
                           std::array<std::uint8_t, B>& data) const;
    template<std::size_t B>
    void SetPointerAndWrite(const std::uint16_t ptrAddress,
                            std::array<uint8_t, B>& data) const;
    void SetPointer(std::uint16_t ptrAddress) const;
    template<std::size_t B>
    void ReadData(std::array<std::uint8_t, B>& data) const;
    template<std::size_t B>
    bool IsCrcInDataValid(const std::array<std::uint8_t, B> &data) const;
    template<std::size_t B>
    void ConvertReadData(const std::array<std::uint8_t, B> &data);
    static std::uint8_t CalculateCrc(std::uint8_t data_0, std::uint8_t data_1);
};

#endif // MASTERTHESISAPP_PARTICLESSENSOR_H
