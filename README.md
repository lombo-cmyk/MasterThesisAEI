# MasterThesisAEI

## Description
### Summary
Built device is able to measure various parameters, while also being portable
 and preferably battery powered. Measured parameters are:
 * Dust content in the air - SparkFun SEN-15103:
     * PM 2.5,
     * PM 10,
 * Carbon dioxide - Figaro CDM7160,
 * Carbon monoxide - Figaro TSG5342,
 * Moisture content - humidity - DHT22,
 * Air temperature, Pressure - LPS331AP
 
Results should be communicated to a different system using e.g. Modbus RTU 
or Modbus via ethernet. Measured values should also be provided to the user 
with 0-10V analog outputs and LCD display.

The work includes testing of sensors in the laboratory, implementation of the 
electronic system, programming of the selected microcontroller.
 
### Devices
Everything is run on ESP32 Devkit v1. The board is capable of
communication through Wi-Fi, UART, I2C and SPI. It can run on 3v3 or 5V. Has
about 30 GPIO pins, each with several usage possibilities. 
[Link](https://docs.zerynth.com/latest/reference/boards/doit_esp32/docs/)
  
There is also a standard 16x2 LCD display provided. It can communicate
with standard compliant with HD44780 controller or through an
additional I2C converter. 

### Language
ESP32 will be programmed using ESP-IDF framework and libraries. ESP-IDF is the 
official development framework for the ESP32 and ESP32-S Series SoCs.
[Link](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
Application code will wrap the legacy c code into a modern C++ used to write 
all application related code.

## Potential sensors to use
It would be nice if sensors could communicate with I2C or would be able to
send readings with analog pins.
* Air quality:
    * ~~First consideration HM3301:~~
        * ~~PM 2.5, 5, 10, 1~500ug/m3~~
        * ~~I2C, address: 0x40~~
        * ~~Price: 135zl~~
        * [Link](https://botland.com.pl/pl/grove-czujniki-gazow-pylow-i-czystosci-powietrza/13812-grove-laserowy-czujnik-pylu-czystosci-powietrza-pm25-hm3301-5v.html)
    * ~~Second consideration PMS5003:~~
        * ~~PM 2.5~~
        * ~~I2C, UART~~
        * ~~Price: 59zł~~
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/6797-czujnik-pylu-czystosci-powietrza-pm25-pms5003-5v-uart.html)
    * Third consideration SparkFun SEN-15103:
        * PM 1.0, 2.5, 4, 10, 1-1000ug/m3
        * UART, I2C
        * Price: 265zł
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/15062-czujnik-pylu-czystosci-powietrza-pm10-pm25-pm4-pm10-sps30-uarti2c-sparkfun-sen-15103.html)
* Carbon dioxide:
    * ~~First consideration SS-CCS811:~~
        * ~~400-29206 ppm~~
        * ~~I2C~~
        * ~~Price: 117zl~~
        * [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/16798-ss-ccs811-i2c-czujnik-dwutlenku-wegla-i-jakosci-powietrza.html)
    * ~~Second consideration MG811:~~
        * ~~350-10000 ppm~~
        * ~~Analog~~
        * ~~Price: 239zl~~
        * [Link](https://botland.com.pl/pl/czujniki-gazow/4487-dfrobot-gravity-czujnik-dwutlenku-wegla-co2-elektrochemiczny.html)
    * ~~Third consideration MH-Z14A WINSEN:~~
        * ~~0-10000 ppm~~
        * ~~UART, PWM, Analog~~
        * ~~Price: 150zl~~
        * [Link](https://www.tme.eu/pl/details/mh-z14a-5kppm/czujniki-gazu/winsen/mh-z14a-5000ppm/)
    * Forth consideration Figaro CDM 7160:
        * 300-5000 ppm (up to 10'000)
        * UART, I2C, PWM
        * Price: around 250zl
        * obsolete due to Fire Incident at Component Supplier Plant
        * [Link](https://www.soselectronic.pl/products/figaro/cdm7160-c00-304480)
        
* Carbon monoxide - optional:
    * ~~Analog - MQ-7-~~ [Link](https://botland.com.pl/pl/czujniki-gazow/239-czujnik-tlenku-wegla-mq-7-polprzewodnikowy.html)
    * Analog - Figaro TSG5342[Link](https://www.figaro.co.jp/en/product/feature/tgs5042_tgs5342-test.html)
* Humidity - Analog HR202 - [Link](https://abc-rc.pl/product-pol-11696-Czujnik-wilgotnosci-HR202-31K-dokladnosc-5-15x12mm.html)
* Temperature
    * ~~Analog LM35DZ:~~ [Link](https://abc-rc.pl/product-pol-7825-Czujnik-temperatury-LM35DZ-analogowy-uklad-scalony-THT.html)
    * ~~One wire* DS18B20:~~ [Link](https://abc-rc.pl/product-pol-6759-DS18B20-Czujnik-temperatury-obudowa-TO92.html)
    * Pressure sensor LPS331AP mentioned below appears to also measure
     temperature, despite of the seller not mentioning it
* Humidity and temperature:
    * First consideration DHT22 25zl, [Link](https://botland.com.pl/pl/czujniki-multifunkcyjne/1612-czujnik-temperatury-i-wilgotnosci-dht22-am2302.html)
    * ~~Second consideration DHT11 8zl,~~ [Link](https://botland.com.pl/pl/czujniki-multifunkcyjne/9301-czujnik-temperatury-i-wilgotnosci-dht11-60c.html)
* Others:
    * Pressure sensor LPS331AP  I2C - [Link](https://botland.com.pl/pl/czujniki-cisnienia/1421-lps331ap-czujnik-cisnienia-i-wysokosci-126kpa-i2cspi-3-5v-pololu-2126.html)
    * ~~Alcohol sensor MQ-3 - Analog -~~ [Link](https://botland.com.pl/pl/czujniki-gazow/5519-czujnik-alkoholu-mq-3-polprzewodnikowy-modul-waveshare-9530.html)
    * ~~Alcohol, benzene, ammonia MQ-135 - Analog -~~ [Link](https://botland.com.pl/pl/czujniki-gazow/5520-czujnik-alkoholu-benzenu-amoniaku-mq-135-polprzewodnikowy-modul-waveshare-9528.html)
    * ~~LPG, i-butane, propane, methane ,alcohol, Hydrogen MQ2- Analog
     -~~ [Link](https://www.dfrobot.com/index.php?route=product/product&product_id=681&search=MQ2&description=true#.VuZzz_mqqAw)





*One wire is capable of providing communication to more devices with a single
wire. Power can also be provided with the same wire, although it's more 
recommended to use a separate connection for it. 