//
// Created by lukaszk on 08.07.2020.
//
#include "driver/gpio.h"

#ifndef AIRCONTROLLER_PINS_H
#define AIRCONTROLLER_PINS_H

#define FORWARD_BUTTON GPIO_NUM_5
#define LCD_SDA_PIN GPIO_NUM_21
#define LCD_SCL_PIN GPIO_NUM_22
#define LCD_BACKLIGHT_PIN GPIO_NUM_18

#define ESP_INTR_FLAG_DEFAULT 0
#define SAMPLE_PERIOD 1000 // milliseconds
#define DEBOUNCE_TIME 70
#define LCD_COLUMNS 16
#define SECOND 1000
#endif // AIRCONTROLLER_PINS_H
