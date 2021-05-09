from pythonping import ping
from datetime import datetime
from time import sleep
from multiprocessing import Process
from pyModbusTCP.client import ModbusClient
from pymodbus.constants import Endian
from pymodbus.client.sync import ModbusTcpClient
from pymodbus.payload import BinaryPayloadDecoder
from pymodbus.exceptions import ConnectionException
import csv
from msvcrt import kbhit as windows_key_press


def ping_to_file():

    while True:
        if windows_key_press():
            break
        file = open('output.txt', 'a+')
        now = datetime.now()
        current_time = now.strftime("%H:%M:%S")
        file.writelines(f"{current_time}\n")
        ping('192.168.11.3', verbose=True, out=file)
        file.close()
        sleep(5)

def get_decoders(modbusData: list, index: int):
    return BinaryPayloadDecoder.fromRegisters(modbusData[index*2:index*2+2],
                                                     byteorder=Endian.Big,
                                                     wordorder=Endian.Little)

def create_modbus_output_file():
    column_names = ["time", "PM2.5", "PM10", "CO", "CO2", "temperature",
                    "humidity", "pressure"]
    with open("modbus.csv", 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(column_names)


def modbus():
    # client = ModbusClient(host="192.168.11.3", port=502)
    # client.open()
    create_modbus_output_file()
    UNIT = 0x01
    client = ModbusTcpClient(host="192.168.11.3", port=502)
    while True:
        if windows_key_press():
            break
        is_connected = client.connect()
        res = None
        if is_connected:
            try:
                res = client.read_holding_registers(0, 14, unit=UNIT)
            except ConnectionException:
                print("Connection failed")
            if res:
                # for i in range(0, 7):
                decoders = [get_decoders(res.registers, i) for i in range(0, 7)]
                values = [decoder.decode_32bit_float() for decoder in decoders]
                now = datetime.now()
                current_time = now.strftime("%H:%M:%S")
                values.insert(0, current_time)
                print(values)
                with open("modbus.csv", 'a+', newline='') as file:
                    writer = csv.writer(file, )
                    writer.writerow(values)
        client.close()
        sleep(5)

def main():
    p = Process(target=ping_to_file)
    p2 = Process(target=modbus)
    p.start()
    p2.start()
    p.join()
    p2.join()
    print("All preocesses ended")


if __name__ == '__main__':
    main()