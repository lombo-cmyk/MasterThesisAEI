import pandas as pd
import matplotlib.pyplot as plt
from VoltageAnalyzer import create_directories
import matplotlib.dates as mdates
import datetime

file = pd.read_csv("polling/2021_05_11_22_17_32/modbus.csv", sep=",")
columns = file.columns
cwd = create_directories("temp_plots")


x = [datetime.datetime.strptime(elem, '%H:%M:%S') for elem in file['time']]
dupa = list()
date_offset = datetime.timedelta(days=0)
for index, elem in enumerate(x):
    if index > 0:
        if elem.time() < x[index-1].time():
            date_offset = datetime.timedelta(days=1)
    dupa.append(elem + date_offset)

for column in columns[1:]:
    fig, ax = plt.subplots()
    ax.plot(dupa, file[column])
    plt.gcf().autofmt_xdate()

    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    plt.title(f"Values for: {column}")
    fig.savefig(cwd + column + ".png")
    plt.close()
