//
// Created by lukaszk on 16.07.2020.
//

#ifndef APP_TEMPLATE_LCD_H
#define APP_TEMPLATE_LCD_H

#include "driver/i2c.h"
#include "i2c-lcd1602.h"
#include "Definitions.h"
#include "InterruptHandler.h"
#include <string>

class LCD {
public:
    LCD();
    void DisplayWelcomeMessage() const;
    void DisplayCurrentState();
    void GetCurrentMeasurements(std::uint16_t pm25,
                                std::uint16_t pm10,
                                double co,
                                std::uint16_t co2,
                                double t,
                                float h,
                                unsigned int p);

private:
    i2c_lcd1602_info_t* LcdInfo_ = new i2c_lcd1602_info_t;
    bool isBacklight_ = true;
    std::uint64_t backlightTimer_ = esp_timer_get_time();
    InterruptHandler& intHandler_ = InterruptHandler::getInstance();
    double CO_ = 0, temperature_ = 0;
    float humidity_ = 0;
    std::uint16_t PM25_ = 0, PM10_ = 0, CO2_ = 0;
    unsigned int pressure_ = 0;
    template<typename T>
    std::string ConvertNumberToString(T number, std::uint8_t precision) const;
    static void AdjustLine(std::string& line);
    void DisplayLine(std::string& line, std::uint8_t row) const;
    void DisplayTwoLines(std::string& line_1, std::string& line_2) const;
    void DisplayPM25();
    void DisplayPM10();
    void DisplayCO();
    void DisplayCO2();
    void DisplayTemperature();
    void DisplayHumidity();
    void DisplayPressure();

    void Setbacklight(const std::uint16_t& displayState);
};

#endif // APP_TEMPLATE_LCD_H
