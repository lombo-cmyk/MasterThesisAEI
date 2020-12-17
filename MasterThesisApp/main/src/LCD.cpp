//
// Created by lukaszk on 16.07.2020.
//

#include "include/LCD.h"
#include "sdkconfig.h"
#include "smbus.h"
#include "include/Definitions.h"
#include "include/InterruptHandler.h"
#include "string"
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>

LCD::LCD() {
    InitializeConnectionConfiguration();
    i2c_port_t i2c_num = I2C_NUM_0;
    uint8_t address = 0x27;
    auto smbus_info = new smbus_info_t;
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS));
    ESP_ERROR_CHECK(
        i2c_lcd1602_init(LcdInfo_, smbus_info, true, 2, 32, LCD_COLUMNS));

    ESP_ERROR_CHECK(i2c_lcd1602_reset(LcdInfo_));
    i2c_lcd1602_set_backlight(LcdInfo_, isBacklight_);
    DisplayWelcomeMessage();
}

void LCD::InitializeConnectionConfiguration() {
    connectionConfiguration_.mode = I2C_MODE_MASTER;
    connectionConfiguration_.sda_io_num = LCD_SDA_PIN;
    connectionConfiguration_.scl_io_num = LCD_SCL_PIN;
    connectionConfiguration_.sda_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.scl_pullup_en = GPIO_PULLUP_ENABLE;
    connectionConfiguration_.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &connectionConfiguration_);
    i2c_driver_install(I2C_NUM_0, connectionConfiguration_.mode, 0, 0, 0);
}

void LCD::AdjustLine(std::string& line) {
    std::string buf;
    buf.reserve(16);
    std::size_t charsToInsert = LCD_COLUMNS - line.length();
    std::size_t startingChars = std::floor(charsToInsert / 2);
    std::size_t endingChars = charsToInsert - startingChars;
    buf.insert(0, startingChars, ' ');
    buf.insert(startingChars, line);
    buf.insert(std::end(buf), endingChars, ' ');
    line = buf;
}

void LCD::DisplayLine(std::string& line, std::uint8_t row) const {
    std::uint8_t pos = 0;
    if (line.length() > 16) {
        std::string lcd_err = "Line too long!";
        i2c_lcd1602_clear(LcdInfo_);
        DisplayLine(lcd_err, 0);
        throw std::invalid_argument("Line too long!");
    }
    AdjustLine(line);
    for (const char& letter : line) {
        i2c_lcd1602_move_cursor(LcdInfo_, pos++, row);
        i2c_lcd1602_write_char(LcdInfo_, letter);
    }
}
void LCD::DisplayTwoLines(std::string& line_1, std::string& line_2) const {
    DisplayLine(line_1, 0);
    DisplayLine(line_2, 1);
}

template<typename T>
std::string LCD::ConvertNumberToString(T number,
                                       std::uint8_t precision) const {
    std::stringstream stringStream;
    stringStream << std::fixed << std::setprecision(precision) << number;
    return stringStream.str();
}


void LCD::DisplayWelcomeMessage() const {
    std::string welcomeMessage1 = "Welcome to";
    std::string welcomeMessage2 = "MasterThesis v1";
    try {
        DisplayTwoLines(welcomeMessage1, welcomeMessage2);
    } catch (const std::invalid_argument& e) {
        std::cout << "Lcd exception thrown: " << e.what() << std::endl;
    }
}

uint8_t LCD::_wait_for_user(void) {
    uint8_t c = 0;

#ifdef USE_STDIN
    while (!c) {
        STATUS s = uart_rx_one_char(&c);
        if (s == OK) {
            printf("%c", c);
        }
        vTaskDelay(1);
    }
#else
    vTaskDelay(1000 / portTICK_RATE_MS);
#endif
    return c;
}

void LCD::Setbacklight(const std::uint16_t& displayState) {
    uint64_t tenSeconds = 10000000;
    if (displayState == 0 && InterruptHandler::GetLcdBacklight()) {
        i2c_lcd1602_set_backlight(LcdInfo_, true);
        backlightTimer_ = esp_timer_get_time();
        InterruptHandler::SetBacklightFromLcd() = false;
    }
    esp_timer_get_time();
    if (displayState == 0 &&
        (esp_timer_get_time() - backlightTimer_) > tenSeconds) {
        i2c_lcd1602_set_backlight(LcdInfo_, false);
    }
    if (displayState != 0) {
        backlightTimer_ = esp_timer_get_time();
        i2c_lcd1602_set_backlight(LcdInfo_, true);
    }
}

void LCD::DisplayCurrentState() {
    std::uint16_t  currentState = InterruptHandler::GetDisplayState();
    std::string state = "State no";
    std::string noState = ConvertNumberToString(currentState, 1);
    switch (currentState) {
        case 0:
            DisplayPMMeasure_25();
            break;
        case 1:
            DisplayPMMeasure_10();
            break;
        case 2:
            DisplayCOMeasure();
            break;
        case 3:
            DisplayCO2Measure();
            break;
        case 4:
            DisplayTempMeasure();
            break;
        case 5:
            DisplayHumidityMeasure();
            break;
        default:
            DisplayTwoLines(state, noState);
    }
}

void LCD::DisplayPMMeasure_25() {
    std::string firstLine = "PM 2.5";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayPMMeasure_10() {
    std::string firstLine = "PM 10";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayCOMeasure() {
    std::string firstLine = "CO";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayCO2Measure() {
    std::string firstLine = "CO2";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayTempMeasure() {
    std::string firstLine = "Temperature";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayHumidityMeasure() {
    std::string firstLine = "Humidity";
    std::string secondLine = "000";
    DisplayTwoLines(firstLine, secondLine);
}
