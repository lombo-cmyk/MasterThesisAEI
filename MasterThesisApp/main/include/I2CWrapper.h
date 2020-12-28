//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_I2CWRAPPER_H
#define MASTERTHESISAPP_I2CWRAPPER_H

#include "driver/i2c.h"
#include "smbus.h"
#include "Singleton.h"

class I2CWrapper final : public Singleton<I2CWrapper> {
public:
    friend Singleton<I2CWrapper>;

    void ConfigureCommunication();
    void pingDevice(smbus_info_t* busInfo, const char* device) ;
    auto GetsmBusInfoDisplay_() -> smbus_info_t* {
        return smBusInfoDisplay_;
    }

    auto GetsmBusInfoPressure_() -> smbus_info_t* {
        return smBusInfoPressureSensor_;
    }
    auto GetsmBusInfoPm() -> smbus_info_t* {
        return smBusInfoPm_;
    }
private:
    const uint8_t displayAddress_ = 0x27;
    const uint8_t pressureSensorAddress_ = 0x5D; // B9 b1011101 -> last bit is Read(1) / Write(1)
    const uint8_t pmSensorAddress_ = 0x69;
    const uint8_t CO2SensorAddress_ = 0x68;
    const i2c_port_t i2cInterface_ = I2C_NUM_0;
    smbus_info_t* smBusInfoDisplay_ = {};
    smbus_info_t* smBusInfoPressureSensor_ = {};
    smbus_info_t* smBusInfoPm_ = {};
    smbus_info_t* smBusInfoCo2 = {};
    i2c_config_t connectionConfiguration_ = {};
    bool isInit(const smbus_info_t* busInfo,const char* deviceName);
    void ConfigureConnectionDetails();
    void ConfigureDisplayBusInfo();
    void ConfigurePressureSensorBusInfo();
    void ConfigurePmSensorBusInfo();
    void ConfigureCo2SensorBusInfo();
};

#endif // MASTERTHESISAPP_I2CWRAPPER_H
