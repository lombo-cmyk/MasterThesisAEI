# MasterThesisAEI

## Description
### Summary
Built device should be able measure various parameters, while also being
 portable and preferably battery powered. Parameters needed to be measured are:
 * Dust content in the air:
     * PM 2.5,
     * PM 10,
 * Carbon dioxide,
 * Moisture content - humidity,
 * Air temperature
 * Carbon monoxide - optional
 
Results should be communicated to a different system using e.g. Modbus RTU 
or Modbus via ethernet. Measured values should also be provided to the user 
with 0-10V analog outputs and LCD display.

The work includes testing of sensors in the laboratory, implementation of the 
electronic system, programming of the selected microcontroller.
 
### Devices
Everything will run on ESP32 Devkit v1. The board is capable of
communication through Wi-Fi, UART, I2C and SPI. It can ran on 3v3 or 5V. Has
about 30 GPIO pins, each with several usage possibilities. [Link](https://docs.zerynth.com/latest/reference/boards/doit_esp32/docs/)
  
There will also be a standard 16x2 LCD display provided. It can communicate
with standard compliant with HD44780 controller or through an
additional I2C converter. 

### Language
ESP32 will be programmed using ESP-IDF framework and libraries. ESP-IDF is the 
official development framework for the ESP32 and ESP32-S Series SoCs. [Link](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
Application code will wrap the legacy c code into a modern C++ used to write 
all application related code.

## Potential sensors to use
It would be nice if sensors could communicate with I2C or would be able to
send readings with analog pins.
* Air quality:
    * First consideration:
        * PM 2.5, 5, 10, 1~500ug/m3
        * I2C, address: 0x40
        * 135zl
        * [Link](https://botland.com.pl/pl/grove-czujniki-gazow-pylow-i-czystosci-powietrza/13812-grove-laserowy-czujnik-pylu-czystosci-powietrza-pm25-hm3301-5v.html)
    * Second consideration:
        * PM 2.5
        * I2C, UART
        * 59zł
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/6797-czujnik-pylu-czystosci-powietrza-pm25-pms5003-5v-uart.html)
    * Third consideration:
        * PM 1.0, 2.5, 4, 10, 1-1000ug/m3
        * UART, I2C
        * 265zł
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/15062-czujnik-pylu-czystosci-powietrza-pm10-pm25-pm4-pm10-sps30-uarti2c-sparkfun-sen-15103.html)
* Carbon dioxide:
    * First consideration:
        * 400-29206 ppm
        * I2C
        * 117zl
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/16798-ss-ccs811-i2c-czujnik-dwutlenku-wegla-i-jakosci-powietrza.html)
    * Second consideration:
        * 350-10000 ppm
        * Analog
        * 239zl
        * [Link](https://botland.com.pl/pl/czujniki-gazow/4487-dfrobot-gravity-czujnik-dwutlenku-wegla-co2-elektrochemiczny.html)
    * Third consideration
        * 0-10000 ppm
        * UART, PWM, Analog
        * 150zl
        * [Link](https://www.tme.eu/pl/details/mh-z14a-5kppm/czujniki-gazu/winsen/mh-z14a-5000ppm/)
* Humidity - Analog - [Link](https://abc-rc.pl/product-pol-11696-Czujnik-wilgotnosci-HR202-31K-dokladnosc-5-15x12mm.html)
* Temperature
    * Analog: [Link](https://abc-rc.pl/product-pol-7825-Czujnik-temperatury-LM35DZ-analogowy-uklad-scalony-THT.html)
    * One wire*: [Link](https://abc-rc.pl/product-pol-6759-DS18B20-Czujnik-temperatury-obudowa-TO92.html)
* Humidity and temperature:
    * First consideration 25zl, [Link](https://botland.com.pl/pl/czujniki-multifunkcyjne/1612-czujnik-temperatury-i-wilgotnosci-dht22-am2302.html)
    * Second consideration 8zl, [Link](https://botland.com.pl/pl/czujniki-multifunkcyjne/9301-czujnik-temperatury-i-wilgotnosci-dht11-60c.html)
* Carbon monoxide - optional - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/239-czujnik-tlenku-wegla-mq-7-polprzewodnikowy.html)
* Others:
    * Pressure sensor - I2C - [Link](https://botland.com.pl/pl/czujniki-cisnienia/1421-lps331ap-czujnik-cisnienia-i-wysokosci-126kpa-i2cspi-3-5v-pololu-2126.html)
    * Alcohol sensor - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/5519-czujnik-alkoholu-mq-3-polprzewodnikowy-modul-waveshare-9530.html)
    * Alcohol, benzene, ammonia - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/5520-czujnik-alkoholu-benzenu-amoniaku-mq-135-polprzewodnikowy-modul-waveshare-9528.html)
    * LPG, i-butane, propane, methane ,alcohol, Hydrogen - Analog - [Link](https://www.dfrobot.com/index.php?route=product/product&product_id=681&search=MQ2&description=true#.VuZzz_mqqAw)





*One wire is capable of providing communication to more devices with a single
wire. Power can also be provided with the same wire, although it's more 
recommended to use a separate connection for it. 