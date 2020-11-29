# MasterThesisAEI

## Description
Built device should be able measure various parameters, while also being
 portable and preferably battery powered. Parameters needed to be measured are:
 * Dust content in the air:
     * PM 2.5,
     * PM 10,
 * Carbon dioxide,
 * Moisture content,
 * Carbon monoxide - optional,
 * Air temperature
 
Results should be communicated to a different system using e.g. Modbus RTU 
or Modbus via ethernet. Measured values should also be provided to the user 
with 0-10V analog outputs and LCD display.

 The work includes testing of sensors in the laboratory, implementation of the 
 electronic system, programming of the selected microcontroller.


## Potential sensors to use
It would be nice if sensors could communicate with I2C or would be able to
send reading with analog pins.
* Pressure sensor - I2C - [Link](https://botland.com.pl/pl/czujniki-cisnienia/1421-lps331ap-czujnik-cisnienia-i-wysokosci-126kpa-i2cspi-3-5v-pololu-2126.html)
* Humidity - Analog - [Link](https://abc-rc.pl/product-pol-11696-Czujnik-wilgotnosci-HR202-31K-dokladnosc-5-15x12mm.html)
* Temperature
    * Analog: [Link](https://abc-rc.pl/product-pol-7825-Czujnik-temperatury-LM35DZ-analogowy-uklad-scalony-THT.html)
    * One wire*: [Link](https://abc-rc.pl/product-pol-6759-DS18B20-Czujnik-temperatury-obudowa-TO92.html)
* Alcohol sensor - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/5519-czujnik-alkoholu-mq-3-polprzewodnikowy-modul-waveshare-9530.html)
* Alcohol, benzene, ammonia - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/5520-czujnik-alkoholu-benzenu-amoniaku-mq-135-polprzewodnikowy-modul-waveshare-9528.html)
* Air quality - UART - [Link](https://botland.com.pl/pl/czujniki-czystosci-powietrza/6797-czujnik-pylu-czystosci-powietrza-pm25-pms5003-5v-uart.html)
* Carbon monoxide - Analog - [Link](https://botland.com.pl/pl/czujniki-gazow/239-czujnik-tlenku-wegla-mq-7-polprzewodnikowy.html)
* LPG, i-butane, propane, methane ,alcohol, Hydrogen - Analog - [Link](
https://www.dfrobot.com/index.php?route=product/product&product_id=681&search=MQ2&description=true#.VuZzz_mqqAw)





*One wire is capable of providing communication to more devices with a single
wire. Power can also be provided with the same wire, although it's more 
recommended to use a separate connection for it. 