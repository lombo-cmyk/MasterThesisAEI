//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_PRESSURESENSOR_H
#define MASTERTHESISAPP_PRESSURESENSOR_H

#define STATUS_REG 0x27
#define CTRL_REG1 0x20
#define DEVICE_ON_CMD 0b10000000


#include "smbus.h"
#include "I2CWrapper.h"

class PressureSensor {
public:
    PressureSensor();
    void ReadPressure();
private:
    static constexpr std::uint8_t whoIAmReg_ = 0x0F;
    static constexpr std::uint8_t statusReg_ = 0x27;
    static constexpr std::uint8_t ctrlReg1_ = 0x20;
    static constexpr std::uint8_t deviceOnCmd_ = 0b10000000;

    smbus_info_t* PressureCommunicationInfo_ = new smbus_info_t;
    std::uint8_t dataFromSensor_;

    esp_err_t WriteByte(std::uint8_t reg, std::uint8_t data);
    esp_err_t ReadByte(std::uint8_t reg, std::uint8_t &data);
    esp_err_t SetValuesInByte(std::uint8_t reg, uint8_t byte);
    esp_err_t ResetValuesInByte(std::uint8_t reg, uint8_t byte);
};

#endif // MASTERTHESISAPP_PRESSURESENSOR_H
