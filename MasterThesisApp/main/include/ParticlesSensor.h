//
// Created by lukaszk on 22.12.2020.
//

#ifndef MASTERTHESISAPP_PARTICLESSENSOR_H
#define MASTERTHESISAPP_PARTICLESSENSOR_H
#include "smbus.h"
#include <bitset>
#include <array>
class ParticlesSensor {
public:
    ParticlesSensor();
    void PerformReadOut();
    auto GetPM25() const -> const double&{
        return PM10;
    }
    auto GetPM10() const -> const double&{
        return PM10;
    }

private:
    double PM25 = 0, PM10 = 0;
    smbus_info_t* ParticlesCommunicationInfo_;
    std::uint16_t measureStartPtr_ = 0x0010; // write 3 (including crc)
    std::uint16_t measureStopPtr_ = 0x0104;
    std::uint16_t readMeasureDataPtr_ = 0x0300; // ans float 60 / int 30
    std::uint16_t DataReadyFlagPtr_ = 0x0202; // ans 3 bytes

    bool isParticleDataAvailable();
    void StartMeasuring(bool measureFloat);
    void StopMeasuring();
    void SetPointerAndRead(const std::uint16_t ptrAddress,
                           std::uint8_t* data,
                           std::size_t len);
    void SetPointerAndWrite(const std::uint16_t ptrAddress,
                            uint8_t* data,
                            uint8_t len);
    void SetPointer(std::uint16_t ptrAddress);
    void ReadData(std::uint8_t* data, std::size_t len);
    std::uint8_t CalculateCrc(std::uint8_t data_0, std::uint8_t data_1);
};

#endif // MASTERTHESISAPP_PARTICLESSENSOR_H
