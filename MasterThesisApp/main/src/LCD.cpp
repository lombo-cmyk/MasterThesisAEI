//
// Created by lukaszk on 16.07.2020.
//

#include "include/LCD.h"
#include "sdkconfig.h"
#include "include/Definitions.h"
#include "string"
#include "include/I2CWrapper.h"
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>

LCD::LCD() {
    auto& I2cWrapper = I2CWrapper::getInstance();
    i2c_lcd1602_init(LcdInfo_,
                     I2cWrapper.GetsmBusInfoDisplay_(),
                     true,
                     2,
                     32,
                     LCD_COLUMNS);
    i2c_lcd1602_reset(LcdInfo_);
    i2c_lcd1602_set_backlight(LcdInfo_, isBacklight_);
    DisplayWelcomeMessage();
}
void LCD::GetCurrentMeasurements(double pm25,
                                 double pm10,
                                 double co,
                                 double co2,
                                 double t,
                                 double h,
                                 unsigned int p) {
    PM25_=pm25;
    PM10_=pm10;
    CO_=co;
    CO2_=co2;
    temperature_=t;
    humidity_=h;
    pressure_=p;
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
    if (displayState == 0 && intHandler_.GetLcdBacklight()) {
        i2c_lcd1602_set_backlight(LcdInfo_, true);
        backlightTimer_ = esp_timer_get_time();
        intHandler_.SetBacklightFromLcd() = false;
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
    std::uint16_t currentState = intHandler_.GetDisplayState();
    std::string state = "State no";
    std::string noState = ConvertNumberToString(currentState, 1);
    switch (currentState) {
    case 0:
        DisplayPM25();
        break;
    case 1:
        DisplayPM10();
        break;
    case 2:
        DisplayCO();
        break;
    case 3:
        DisplayCO2();
        break;
    case 4:
        DisplayTemperature();
        break;
    case 5:
        DisplayHumidity();
        break;
    case 6:
        DisplayPressure();
        break;
    default:
        DisplayTwoLines(state, noState);
    }
}

void LCD::DisplayPM25() {
    std::string firstLine = "PM 2.5";
    std::string secondLine = ConvertNumberToString(PM25_, 2);
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayPM10() {
    std::string firstLine = "PM 10";
    std::string secondLine = ConvertNumberToString(PM10_, 2);
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayCO() {
    std::string firstLine = "CO";
    std::string secondLine = ConvertNumberToString(CO_, 2);
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayCO2() {
    std::string firstLine = "CO2";
    std::string secondLine = ConvertNumberToString(CO2_, 2);
    DisplayTwoLines(firstLine, secondLine);
}

void LCD::DisplayTemperature() {
    std::string firstLine = "Temperature";
    std::string secondLine = ConvertNumberToString(temperature_, 2);
    DisplayTwoLines(firstLine, secondLine);
}
void LCD::DisplayHumidity() {
    std::string firstLine = "Humidity";
    std::string secondLine = ConvertNumberToString(humidity_, 2);
    DisplayTwoLines(firstLine, secondLine);
}
void LCD::DisplayPressure() {
    std::string firstLine = "Pressure";
    std::string secondLine = ConvertNumberToString(pressure_, 2);
    DisplayTwoLines(firstLine, secondLine);
}
