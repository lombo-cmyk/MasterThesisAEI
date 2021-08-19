# Data read results:
Results were obtained using QModMaster v0.5.2-3 in the following configuration:

* Slave IP: as given
* TCP Port: 502
* Modbus Mode: TCP,
* Unit ID: 1,
* Scan Rate (ms): N/A
* Function Code: Read Holding Registers (0x03)
* Start Address: 0
* Number of Registers: 16
* Data Format: Hex

> Scan rate could be customized depending on needs, on demand scan can also be
 used to obtain results. 

Exemplary collected data can consist of the following:

 |      |      |      |      |      |      |      |      |      
 |:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:| 
 | 0000 | 40A0 | 0000 | 40A0 | 4000 | 4452 | F777 | 41C8 |
 | 0000 | 4264 | 4000 | 4474 | C000 | 4491 | 999A | 41CD |
 
 The device keeps floating point numbers in the registers. The order is as
  follows: Big Endian byte order and Little Endian word order
  (Mid-Little Endian (CDAB)) so in order to parse it each 2 words (4 bytes)
  should be converted into a total of 8 values with words reversed (using i.e. 
  [this floating point to hex converter](https://gregstoll.com/~gregstoll/floattohex/) 
   or any other consistent with IEEE 753 standard:

  * PM2.5: 0x40A00000 -> 5
  * PM10: 0x40A00000 -> 5
  * CO2: 0x44524000 -> 841
  * Temp(DHT): 0x41C8F777 -> 25.1208
  * Hum: 0x42640000 -> 57
  * Pressure: 0x44744000 -> 977
  * CO: 0x4491C000 -> 1166
  * Temp(LP): 0x41CD999A -> 25.7