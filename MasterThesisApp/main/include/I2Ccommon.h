//
// Created by lukaszk on 26.12.2020.
//

#ifndef MASTERTHESISAPP_I2CCOMMON_H
#define MASTERTHESISAPP_I2CCOMMON_H

#include "esp_log.h"
#include "esp_err.h"

class I2Ccommon {
public:
    auto GetCurrentError() const -> const esp_err_t& {
        return currentError_;
    }
    auto GetPreviousRealError() const -> const esp_err_t& {
        return previousRealError_;
    }

protected:
    esp_err_t currentError_ = 0;
    esp_err_t previousRealError_ = 0;

    bool IsErrorInCommunication(esp_err_t error);
};

#endif // MASTERTHESISAPP_I2CCOMMON_H
