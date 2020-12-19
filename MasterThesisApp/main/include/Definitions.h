//
// Created by lukaszk on 08.07.2020.
//
#include "driver/gpio.h"

#ifndef AIRCONTROLLER_PINS_H
#define AIRCONTROLLER_PINS_H

#define DO_NOT_USE GPIO_NUM_5 // well, this one is broken, 5V too strong
#define FORWARD_BUTTON GPIO_NUM_18
#define BUTTON_2 GPIO_NUM_19
#define LCD_SDA_PIN GPIO_NUM_21
#define LCD_SCL_PIN GPIO_NUM_22
#define LCD_BACKLIGHT_PIN GPIO_NUM_18

#define ESP_INTR_FLAG_DEFAULT 0
#define SAMPLE_PERIOD 1000 // milliseconds
#define DEBOUNCE_TIME 10 // 70 needed w/o proper HW debouncing
#define LCD_COLUMNS 16
#define SECOND 1000
#endif // AIRCONTROLLER_PINS_H
