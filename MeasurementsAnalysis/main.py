import pandas as pd
import seaborn as sns
from scipy import stats
import numpy as np
import os
from datetime import datetime
import matplotlib.pyplot as plt

measurements_path = "measurements/OneTimeMeasureVoltage.csv"
difference_col = "Difference"


def create_directories():
    script_dir = os.path.dirname(__file__)
    results_dir = os.path.join(script_dir, 'temp_plots/')
    current_working_dir = os.path.join(results_dir, datetime.now().strftime(
        "%Y_%m_%d_%H_%M_%S/"))
    if not os.path.isdir(results_dir):
        os.makedirs(results_dir)
    os.makedirs(current_working_dir)
    return current_working_dir


plots_directory = create_directories()


def create_boxplot(title: str, filename: str, xlabel: str,
                   ylabel: str, plot_df: pd.DataFrame):
    sns.boxplot(x="variable", y="value", data=pd.melt(plot_df),
                showmeans=True,
                meanprops={"marker": "o",
                           "markerfacecolor": "white",
                           "markeredgecolor": "black",
                           "markersize": "10"}
                )
    plt.title(title, wrap=True)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    save_fig(filename)
    plt.close()


def save_fig(name: str):
    plt.savefig(f"{plots_directory}{name}.png")


def drop_outliers(measurements: pd.DataFrame):
    z_score = np.abs(stats.zscore(measurements[difference_col]))
    threshold = 1.0
    indexes_to_drop = np.where(z_score > threshold)[0]
    return measurements.drop(index=indexes_to_drop)


def main():
    measurements = pd.read_csv(measurements_path, sep=";")
    raw, esp, meter = measurements.columns
    measurements[difference_col] = measurements[esp] - measurements[meter]

    ylabel = "Measured difference"
    title = "Differences between values measured by ESP and Multimeter [mV]"

    filename = "undropped_differences"
    xlabel = f"Median: {measurements[difference_col].median()}, " \
             f"Mean: {round(measurements[difference_col].mean(), 2)}"
    create_boxplot(f"Undropped: {title}", filename, xlabel, ylabel,
                   measurements[[difference_col]])

    measurements_out = drop_outliers(measurements)
    xlabel = f"Median: {measurements_out[difference_col].median()}, " \
             f"Mean: {round(measurements_out[difference_col].mean(),2 )}"
    filename = "dropped_differences"
    create_boxplot(f"Dropped: {title}", filename, xlabel, ylabel,
                   measurements_out[[difference_col]])

    merged_df = pd.DataFrame({'Undropped': measurements[difference_col], 'Dropped': measurements_out[difference_col]})
    xlabel = ""
    filename = "comparison_differences"
    create_boxplot(title, filename, xlabel, ylabel,
                   merged_df)

    plt.scatter(measurements[esp]-20, range(0, measurements[esp].shape[0]))
    plt.scatter(measurements[meter], range(0, measurements[esp].shape[0]))
    plt.show()


if __name__ == '__main__':
    main()

