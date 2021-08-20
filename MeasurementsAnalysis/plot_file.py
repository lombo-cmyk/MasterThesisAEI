import datetime
import pandas as pd
import matplotlib.dates as mdates
import matplotlib.pyplot as plt

from common import create_directories, Columns, COLUMN_NAMES

file = pd.read_csv("results/polling/2021_05_11_22_17_32/dupa.csv", sep=",")
cwd = create_directories("temp_plots")


x = [datetime.datetime.strptime(elem, '%H:%M:%S')
     for elem in file[COLUMN_NAMES.get(Columns.TIME)]]
dupa = list()
date_offset = datetime.timedelta(days=0)
for index, elem in enumerate(x):
    if index > 0:
        if elem.time() < x[index-1].time():
            date_offset = datetime.timedelta(days=1)
    dupa.append(elem + date_offset)

y_label = ["PM2.5 Concentration [ppm]", "", "", "Carbon dioxide [ppm]",
           "Temperature[°C]", "Relative humidity", ""]
param_colums = list(COLUMN_NAMES.values())
param_colums.remove(COLUMN_NAMES.get(Columns.TIME))
for column, y in zip(param_colums, y_label):
    fig, ax = plt.subplots()
    ax.scatter(dupa, file[column], marker='x', s=1, c='black')
    plt.gcf().autofmt_xdate()

    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    # plt.title(f"Values for: {column}")
    plt.xlabel("Time [HH:MM]", fontsize=20)
    plt.ylabel(y, fontsize=20)
    plt.grid()
    fig.savefig(cwd + column + ".png")
    plt.close()
