//
// Created by lukaszk on 02.01.2021.
//

#include "include/Co2Sensor.h"
#include "include/I2CWrapper.h"
#include "smbus.h"
#include <iostream>
#include "include/Converter.h"

Co2Sensor::Co2Sensor() {
    auto& i2cWrapper = I2CWrapper::getInstance();
    Co2CommunicationInfo_ = i2cWrapper.GetsmBusInfoCo2();
}
bool Co2Sensor::StartMeasuring() {
    bool ret = true;
    std::uint8_t startCommand = 0x06;
    esp_err_t error;
    if (!IsSensorBusy()) {
        error = smbus_write_byte(Co2CommunicationInfo_,
                                 operatingModePtr_,
                                 startCommand);
        ret = IsErrorInCommunication(error);
    }
    return ret;
}
bool Co2Sensor::StopMeasuring() {
    bool ret = true;
    std::uint8_t startCommand = 0x00;
    esp_err_t error;
    if (!IsSensorBusy()) {
        error = smbus_write_byte(Co2CommunicationInfo_,
                                 operatingModePtr_,
                                 startCommand);
        ret = IsErrorInCommunication(error);
    }
    return ret;
}
bool Co2Sensor::PerformReadout() {
    bool ret = true;
    esp_err_t error1, error2;
    std::uint8_t data0 = 0, data1 = 0;
    if (IsSensorBusy()) {
        error1 = smbus_read_byte(Co2CommunicationInfo_,
                                 lowCo2DataPtr_,
                                 &data0);
        error2 = smbus_read_byte(Co2CommunicationInfo_,
                                 highCo2DataPtr_,
                                 &data1);
        ret = IsErrorInCommunication(error1) || IsErrorInCommunication(error2);
    }
    if (!ret) {
        std::uint16_t dd;
        dd = (data1 << 8u) + data0;
        std::cout << "Co2 readout: " << static_cast<int>(dd) << " End"
                  << std::endl;
        Co2Value_ = dd;
    }
    return ret;
}
bool Co2Sensor::IsDeviceOn() {
    return false;
}
bool Co2Sensor::IsSensorBusy() {
    std::uint8_t data;
    smbus_read_byte(Co2CommunicationInfo_, readStatusPtr_, &data);
    std::bitset<8> a = ConvertToBitset(data);
    return static_cast<bool>(a[7]);
}
