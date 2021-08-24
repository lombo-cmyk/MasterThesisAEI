import datetime
import pandas as pd
import matplotlib.dates as mdates
import matplotlib.pyplot as plt

from common import create_directories, Columns, COLUMN_NAMES


def adjust_date(time_column: pd.DataFrame):
    date_offset = datetime.timedelta(days=1)
    new_day_index = 0
    time_stamp = [datetime.datetime.strptime(elem, '%H:%M:%S')
                  for elem in time_column]

    for index, elem in enumerate(time_stamp[1:], 1):
        if elem.time() < time_stamp[index - 1].time():
            new_day_index = index
            break

    old_day = time_stamp[:new_day_index]
    new_day = [elem + date_offset for elem in time_stamp[new_day_index:]]
    return old_day + new_day


def main():
    y_label_dict = {Columns.PM25: "PM2.5 Concentration [ppm]",
                    Columns.CO2: "Carbon dioxide [ppm]",
                    Columns.TEMPERATURE: "Temperature[°C]",
                    Columns.HUMIDITY: "Relative humidity"}

    file = pd.read_csv("measurements/24h_run.csv", sep=",")
    cwd = create_directories("temp_plots")

    time_of_day = adjust_date(time_column=file[COLUMN_NAMES.get(Columns.TIME)])

    for key, y_label in y_label_dict.items():
        col_name = COLUMN_NAMES.get(key)
        fig, ax = plt.subplots()
        ax.scatter(time_of_day, file[col_name], marker='x', s=1, c='black')
        plt.gcf().autofmt_xdate()

        ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

        plt.title(f"Values for {col_name}")
        plt.xlabel("Time [HH:MM]", fontsize=20)
        plt.ylabel(y_label, fontsize=20)
        plt.grid()
        fig.savefig(cwd + col_name + ".png")
        plt.close()


if __name__ == '__main__':
    main()
