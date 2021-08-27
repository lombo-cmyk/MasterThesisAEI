import pandas as pd
import datetime
import matplotlib.pyplot as plt
from common.free_functions import create_directories
import matplotlib.dates as mdates

from common import Columns, COLUMN_NAMES
cwd = create_directories("temp_plots")

data = pd.read_csv("measurements/long_run.csv", sep=",")

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


def create_latex_measurement_error_table(data__: pd.DataFrame) -> pd.DataFrame:
    line_ending = r"\\ \hline"
    print("temp_set  | abs_erorr_lp | rel_erorr_lp | "
          "abs_erorr_dht_temp | rel_erorr_dht_temp | "
          "abs_error_dht_hum | rel_erorr_dht_hum")
    for index, temp_set, temp_ref, hum_ref in zip(FILE_INDEXES,
                                                  TEMP_SET_POINTS,
                                                  TEMP_REF_VAL,
                                                  HUM_REF_VAL):
        mean_dict = calc_mean_and_std_values(data__, index)
        error_dict = calculate_errors(mean_dict, temp_ref, hum_ref)

        print(f"{temp_set} ", end='')
        for col in [TEMP_COL, TEMP_DHT_COL, HUM_DHT_COL]:
            value = error_dict.get(col)
            print(f"& {value[0]:.2f} & {value[1]:.2f} ", end='')
        print(line_ending)


create_latex_measurement_error_table(data)

# *****************************************************************
# ********* File is partially refactored up to this point *********
# *****************************************************************

dht_temp_offset = 1.20
dht_temp_scale = TEMP_REF_VAL[-1] / (data[TEMP_DHT_COL][FILE_INDEXES[-1]] -
                                     dht_temp_offset)

timestamps = [datetime.datetime.strptime(elem, '%H:%M:%S') for elem in data['time']]

date_offset = datetime.timedelta(hours=timestamps[0].hour)

timestamps = [d - date_offset for d in timestamps]
fig, ax = plt.subplots()
ax.plot(timestamps[0:1695 + 10], (data[TEMP_DHT_COL][0:1695 + 10]), label="Raw "
                                                            "measurements", c='b')
dht_temperature_corrected = (data[TEMP_DHT_COL][0:1695 + 10] -
                             dht_temp_offset) * dht_temp_scale
ax.plot(timestamps[0:1695 + 10], dht_temperature_corrected,
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
fig.savefig(f"{cwd}co2_values.png", dpi=300)


line_ending = r"\\ \hline"
print(30 * "# ")
for index, temp_set, temp_ref in zip(FILE_INDEXES, TEMP_SET_POINTS, TEMP_REF_VAL):
    measured_temp_dht = dht_temperature_corrected[index]
    abs_erorr_dht_t = abs(measured_temp_dht - temp_ref)
    rel_erorr_dht_t = abs_erorr_dht_t / temp_ref * 100

    print(f"{temp_set} & {abs_erorr_dht_t:.2f} & {rel_erorr_dht_t:.2f}   " +
          line_ending)