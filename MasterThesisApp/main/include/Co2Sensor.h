//
// Created by lukaszk on 02.01.2021.
//

#ifndef MASTERTHESISAPP_CO2SENSOR_H
#define MASTERTHESISAPP_CO2SENSOR_H

#include <bitset>
#include "I2Ccommon.h"
#include "smbus.h"
#include <cstdint>


class Co2Sensor: private I2Ccommon {
public:
    Co2Sensor();
    bool StartMeasuring();
    bool StopMeasuring();
    bool PerformReadout();

    auto GetCo2Value() const -> const std::uint16_t &{
        return Co2Value_;
    }

private:
    smbus_info_t* Co2CommunicationInfo_;
    std::uint16_t Co2Value_ = 0;
    bool IsDeviceOn();
    bool IsSensorBusy();
    static constexpr std::uint8_t operatingModePtr_ = 0x01;
    static constexpr std::uint8_t readStatusPtr_ = 0x02;
    static constexpr std::uint8_t lowCo2DataPtr_ = 0x03;
    static constexpr std::uint8_t highCo2DataPtr_ = 0x04;
};

#endif // MASTERTHESISAPP_CO2SENSOR_H
