import csv
from argparse import ArgumentParser
from datetime import datetime
from msvcrt import kbhit as windows_key_press
from multiprocessing import Event, Process
from pymodbus.constants import Endian
from pymodbus.client.sync import ModbusTcpClient
from pymodbus.payload import BinaryPayloadDecoder
from pymodbus.exceptions import ConnectionException
from pythonping import ping

from common import create_directories, COLUMN_NAMES


def ping_to_file(cwd: str, event: Event, ip: str):
    while True:
        if event.wait(timeout=5):
            print("Ping task done...")
            break

        file = open(f"{cwd}/output.txt", 'a+')
        now = datetime.now()
        current_time = now.strftime("%H:%M:%S")
        file.writelines(f"{current_time}\n")
        ping(ip, verbose=True, out=file)
        file.close()


def get_decoders(modbus_data: list, index: int):
    return BinaryPayloadDecoder.fromRegisters(
        modbus_data[index * 2:index * 2 + 2],
        byteorder=Endian.Big,
        wordorder=Endian.Little)


def create_modbus_output_file(cwd: str):
    with open(f"{cwd}/modbus.csv", 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(COLUMN_NAMES.values())


def modbus(cwd: str, event: Event, ip: str):
    create_modbus_output_file(cwd)
    slave_address = 0x01
    client = ModbusTcpClient(host=ip, port=502, timeout=25)
    while True:
        if event.wait(timeout=5):
            print("Modbus task done...")
            break
        is_connected = client.connect()
        res = None
        if is_connected:
            try:
                res = client.read_holding_registers(0, 18, unit=slave_address)
            except ConnectionException:
                print("Connection failed")
            if res:
                decoders = [get_decoders(res.registers, i) for i in
                            range(0, 9)]
                values = [decoder.decode_32bit_float() for decoder in decoders]
                now = datetime.now()
                current_time = now.strftime("%H:%M:%S")
                values.insert(0, current_time)
                print(values)
                with open(f"{cwd}/modbus.csv", 'a+', newline='') as file:
                    writer = csv.writer(file, )
                    writer.writerow(values)
        client.close()


def finisher(event: Event):
    while True:
        if windows_key_press():
            event.set()
            print("Finishing in progress...")
            break


def main():
    argpars = ArgumentParser()
    argpars.add_argument("--ip", help="IP to ping & read data",
                         required=True)
    args = argpars.parse_args()
    cwd = create_directories("polling")
    event = Event()
    p = Process(target=ping_to_file, args=(cwd, event, args.ip))
    p2 = Process(target=modbus, args=(cwd, event, args.ip))
    p3 = Process(target=finisher, args=(event,))
    p.start()
    p2.start()
    p3.start()
    p.join()
    p2.join()
    p3.join()
    print("All processes ended")


if __name__ == '__main__':
    main()
