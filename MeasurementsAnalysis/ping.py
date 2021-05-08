from pythonping import ping
from datetime import datetime
from time import sleep


while(True):
    file = open('output.txt', 'a+')
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    file.writelines(f"{current_time}\n")
    ping('192.168.1.21', verbose=True, out=file)
    file.close()
    sleep(15)

