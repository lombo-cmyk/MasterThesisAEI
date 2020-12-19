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
    auto GetsmBusInfoDisplay_() -> smbus_info_t* {
        return smBusInfoDisplay_;
    }

private:
    const uint8_t displayAddress_ = 0x27;
    const i2c_port_t i2cInterface_ = I2C_NUM_0;
    smbus_info_t* smBusInfoDisplay_ = {};
    i2c_config_t connectionConfiguration_ = {};
    void ConfigureConnectionDetails();
    void ConfigureDisplaySettings();
};

#endif // MASTERTHESISAPP_I2CWRAPPER_H
