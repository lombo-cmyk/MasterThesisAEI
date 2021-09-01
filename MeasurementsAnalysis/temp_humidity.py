# TODO: extract latex table generator to something universal

import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd

from common import Columns, COLUMN_NAMES, create_directories

cwd = create_directories("temp_plots")

TEMP_COL = COLUMN_NAMES.get(Columns.TEMPERATURE)
TEMP_DHT_COL = COLUMN_NAMES.get(Columns.TEMP_DHT)
HUM_DHT_COL = COLUMN_NAMES.get(Columns.HUMIDITY)

# *****************************************************************
# *** Below values were taken during measurements in laboratory ***
# ****** FILE_INDEXES are indexes, where values were stable. ******
# *****************************************************************
FILE_INDEXES = [313, 574, 1249, 1695]
TEMP_SET_POINTS = [10.0, 5.0, 35.0, 50.0]
TEMP_REF_VAL = [10.3, 5.6, 34.7, 49.0]
HUM_REF_VAL = [60.8, 65, 56, 40.9]


def calc_mean_and_std_values(data__: pd.DataFrame, index: int) -> dict:
    mean_dict = {}
    for col in [TEMP_COL, TEMP_DHT_COL, HUM_DHT_COL]:
        mean_dict[col] = data__[col][index - 3:index + 3].mean()
    return mean_dict


def calculate_errors(mean_dict: dict,
                     temp_ref: float,
                     hum_ref: float) -> dict:
    error_dict = {}
    iter_list = [(TEMP_COL, temp_ref),
                 (TEMP_DHT_COL, temp_ref),
                 (HUM_DHT_COL, hum_ref)]

    for col, ref_value in iter_list:
        abs_error = abs(mean_dict[col] - ref_value)
        rel_error = abs_error/ref_value * 100
        error_dict[col] = (abs_error, rel_error)
    return error_dict


def create_latex_measurement_error_table(data__: pd.DataFrame) -> None:
    line_ending = r"\\ \hline"
    columns = ["temp_set", "abs_erorr_lp", "rel_erorr_lp",
               "abs_erorr_dht_temp", "rel_erorr_dht_temp",
               "abs_error_dht_hum", "rel_erorr_dht_hum"]
    longest_word = len(max(columns, key=len))
    row_format = ("{:>%s}" % str(longest_word + 1)) * (len(columns))
    cell_format = "{:>%s}" % str(longest_word + 1)
    print(row_format.format(*columns))
    for index, temp_set, temp_ref, hum_ref in zip(FILE_INDEXES,
                                                  TEMP_SET_POINTS,
                                                  TEMP_REF_VAL,
                                                  HUM_REF_VAL):
        mean_dict = calc_mean_and_std_values(data__, index)
        error_dict = calculate_errors(mean_dict, temp_ref, hum_ref)

        print(cell_format.format(temp_set), end='')
        for col in [TEMP_COL, TEMP_DHT_COL, HUM_DHT_COL]:
            value = error_dict.get(col)
            print(cell_format.format(f"& {value[0]:.2f}"), end='')
            print(cell_format.format(f"& {value[1]:.2f} "), end='')
        print(line_ending)


def get_dht_temp_offset(data: pd.DataFrame) -> float:
    min_ref_index = TEMP_REF_VAL.index(min(TEMP_REF_VAL))
    data_ref_index = FILE_INDEXES[min_ref_index]
    offset = data[TEMP_DHT_COL][data_ref_index] - TEMP_REF_VAL[min_ref_index]
    return round(offset, 2)


def get_timestamps(data: pd.DataFrame) -> list:
    timestamps = [datetime.datetime.strptime(elem, '%H:%M:%S') for elem in
                  data['time']]
    date_offset = datetime.timedelta(hours=timestamps[0].hour)
    timestamps = [d - date_offset for d in timestamps]
    return timestamps


def correct_dht_measurements(data: pd.DataFrame) -> pd.Series:
    dht_temp_offset = get_dht_temp_offset(data)
    dht_temp_scale = TEMP_REF_VAL[-1] / (data[TEMP_DHT_COL][FILE_INDEXES[-1]] -
                                         dht_temp_offset)
    dht_temperature_corrected = (data[TEMP_DHT_COL][:max(FILE_INDEXES) + 10] -
                                 dht_temp_offset) * dht_temp_scale
    return dht_temperature_corrected


def plot_dht_measurements(data: pd.DataFrame,
                          corrected_temp: pd.Series) -> None:
    fig, ax = plt.subplots()

    timestamps = get_timestamps(data)

    ax.plot(timestamps[:max(FILE_INDEXES) + 10],
            data[TEMP_DHT_COL][:max(FILE_INDEXES) + 10],
            label="Raw measurements", c='b')

    ax.plot(timestamps[:max(FILE_INDEXES) + 10],
            corrected_temp,
            label="Corrected measurements", c='g')

    time_scatter = [timestamps[index] for index in FILE_INDEXES]
    ax.scatter(time_scatter, TEMP_REF_VAL, marker='x', s=60, c='r',
               label="Reference values")

    plt.gcf().autofmt_xdate()
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    plt.xlabel("Time [HH:MM]", fontsize=20)
    plt.ylabel("Temperature [°C]", fontsize=20)
    plt.grid()
    plt.legend()
    fig.savefig(f"{cwd}dht_temp.png", dpi=300)


def create_latex_corrected_dht_table(temp_corrected: pd.Series):
    line_ending = r" \\ \hline"
    columns = ["temp_set", "absolute error", "relative error"]
    longest_word = len(max(columns, key=len))
    row_format = ("{:>%s}" % str(longest_word + 1)) * (len(columns))
    print(row_format.format(*columns))
    for index, temp_set, temp_ref in zip(FILE_INDEXES, TEMP_SET_POINTS,
                                         TEMP_REF_VAL):
        measured_temp_dht = temp_corrected[index]
        abs_erorr_dht_t = round(abs(measured_temp_dht - temp_ref), 2)
        rel_erorr_dht_t = round(abs_erorr_dht_t / temp_ref * 100, 2)
        print(row_format.format(temp_set, "& " + str(abs_erorr_dht_t),
                                "& " + str(rel_erorr_dht_t)) + line_ending)


def main():
    data = pd.read_csv("measurements/long_run.csv", sep=",")

    create_latex_measurement_error_table(data)
    print(30 * "# ")
    dht_corrected_temperature = correct_dht_measurements(data)
    plot_dht_measurements(data, dht_corrected_temperature)
    create_latex_corrected_dht_table(dht_corrected_temperature)


if __name__ == '__main__':
    main()
