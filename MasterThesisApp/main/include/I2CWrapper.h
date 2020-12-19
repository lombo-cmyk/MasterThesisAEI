//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_I2CWRAPPER_H
#define MASTERTHESISAPP_I2CWRAPPER_H

#include "driver/i2c.h"
#include "smbus.h"

struct I2CWrapper {
public:
    static void Start();
    static auto GetsmBusInfoDisplay_() -> smbus_info_t* {
        return smBusInfoDisplay_;
    }
private:

    static const i2c_port_t i2cInterface = I2C_NUM_0;
    static i2c_config_t connectionConfiguration_;
    static const uint8_t displayAddress_ = 0x27;
    static smbus_info_t* smBusInfoDisplay_;
    static void ConfigureConnection();
    static void ConfigureDisplaySettings();
};

#endif // MASTERTHESISAPP_I2CWRAPPER_H
