//
// Created by lukaszk on 08.07.2020.
//
#include "driver/gpio.h"
#include <string>
#ifndef AIRCONTROLLER_PINS_H
#define AIRCONTROLLER_PINS_H

#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK true

#define DO_NOT_USE GPIO_NUM_5 // well, this one is broken, 5V too strong
#define FORWARD_BUTTON GPIO_NUM_18
#define CO2_BUSY_BUTTON GPIO_NUM_19
#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22
#define DHT_PIN GPIO_NUM_23

#define ESP_INTR_FLAG_DEFAULT 0
#define SAMPLE_PERIOD 1000 // milliseconds
#define DEBOUNCE_TIME 10 // 70 needed w/o proper HW debouncing
#define LCD_COLUMNS 16
#define SECOND 100

enum regIndex {
    indexPm25,
    indexPm10,
    indexCo,
    indexCo2,
    indexTemperature,
    indexHumidity,
    indexPressure
};

#define ESP_DEVICE_BUSY 0x111

constexpr char deviceLCD[] = "LCD";
constexpr char devicePressSens[] = "PressureSensor";
constexpr char devicePmSens[] = "ParticlesSensor";
constexpr char deviceCo2Sens[] = "Co2_Sensor";
#endif // AIRCONTROLLER_PINS_H
