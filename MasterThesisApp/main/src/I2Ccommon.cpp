//
// Created by lukaszk on 26.12.2020.
//

#include "include/I2Ccommon.h"
#include "include/Definitions.h"

bool I2Ccommon::IsErrorInCommunication(esp_err_t error) {
    bool ret = true;
    currentError_ = error;
    if (error != 0) {
        previousRealError_ = error;
        switch (error) {
        case ESP_FAIL:
            ESP_LOGE(devicePmSens, "ESP FAIL generic error");
            break;
        case ESP_ERR_TIMEOUT:
            ESP_LOGE(devicePmSens, "Operation Timeout - no communication");
            break;
        case ESP_ERR_INVALID_CRC:
            ESP_LOGE(devicePmSens, "Invalid CRC");
            break;
        case ESP_ERR_INVALID_RESPONSE:
            ESP_LOGE(devicePmSens, "Invalid response");
            break;
        default:
            ESP_LOGE(devicePmSens, "Unexpected error: %d", error);
        }
    } else {
        ret = false;
    }
    return ret;
}
