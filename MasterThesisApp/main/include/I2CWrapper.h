//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_I2CWRAPPER_H
#define MASTERTHESISAPP_I2CWRAPPER_H

#include "driver/i2c.h"

class I2CWrapper{
public:
    I2CWrapper();
private:
//    void BusInit();
    void ConfigureConnection();
    i2c_config_t connectionConfiguration_ = {};

};


#endif //MASTERTHESISAPP_I2CWRAPPER_H
