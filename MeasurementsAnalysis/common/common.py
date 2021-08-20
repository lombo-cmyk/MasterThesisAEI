from enum import Enum


class Columns(Enum):
    TIME = 1
    PM25 = 2
    PM10 = 3
    CO = 4
    CO2 = 5
    TEMPERATURE = 6
    HUMIDITY = 7
    PRESSURE = 8
    CO_MVOLTS = 9
    TEMP_DHT = 10


COLUMN_NAMES = {col: col.name.lower() for col in Columns}
