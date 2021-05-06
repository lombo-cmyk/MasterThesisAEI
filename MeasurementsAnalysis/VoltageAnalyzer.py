import pandas as pd
import seaborn as sns
from scipy import stats
import numpy as np
import os
from datetime import datetime
import matplotlib.pyplot as plt


class VoltageAnalyzer:

    diff_col = "Difference"

    def __init__(self, working_dir: str, file: str):
        self.working_directory = working_dir
        self.file_path = file
        self.results_raw = self.__get_measurements()
        self.raw, self.esp, self.meter, *others = self.__get_column_names()
        if others:
            raise RuntimeError('Not all columns were properly unpacked')
        self.results_raw[self.diff_col] = self.results_raw[self.esp] - \
                                          self.results_raw[self.meter]
        self.results_processed = None

    def __get_measurements(self):
        return pd.read_csv(self.file_path, sep=";")

    def __get_column_names(self):
        return self.results_raw.columns

    def __create_boxplots(self, title: str, filename: str, xlabel: str,
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
        self.__save_fig(filename)

    def __save_fig(self, name: str):
        plt.savefig(f"{self.working_directory}{name}.png")
        plt.close()

    def __drop_outliers(self):
        z_score = np.abs(stats.zscore(self.results_raw[self.diff_col]))
        threshold = 1
        indexes_to_drop = np.where(z_score > threshold)[0]
        return self.results_raw.drop(index=indexes_to_drop), indexes_to_drop

    def create_raw_values_boxplot(self):
        filename = "undropped_differences"
        title = "Undropped: Differences between values measured by ESP and " \
                "Multimeter [mV]"
        ylabel = "Measured difference"
        xlabel = f"Median: {self.results_raw[self.diff_col].median()}, " \
                 f"Mean: {round(self.results_raw[self.diff_col].mean(), 2)}"
        self.__create_boxplots(title, filename, xlabel, ylabel,
                               self.results_raw[[self.diff_col]])

    def process_results(self):
        self.results_processed, index = self.__drop_outliers()
        print(f"Dropped {len(index)} rows: {index}")

    def create_processed_values_boxplot(self):
        filename = "dropped_differences"
        title = "Dropped: Differences between values measured by ESP and " \
                "Multimeter [mV]"
        ylabel = "Measured difference"
        xlabel = f"Median: {self.results_processed[self.diff_col].median()}," \
                 f" Mean:" \
                 f" {round(self.results_processed[self.diff_col].mean(), 2)}"
        self.__create_boxplots(title, filename, xlabel, ylabel,
                       self.results_processed[[self.diff_col]])

    def create_comparison_boxplot(self):
        merged_df = pd.DataFrame({'Undropped': self.results_raw[self.diff_col],
                                  'Dropped':
                                      self.results_processed[self.diff_col]})
        filename = "comparison_differences"
        ylabel = "Measured difference"
        xlabel = ""
        title = "Dropped: Differences between values measured by ESP and " \
                "Multimeter [mV]"
        self.__create_boxplots(title, filename, xlabel, ylabel, merged_df)

    def create_scatter_comparison_raw(self):
        dot_size = 80
        number_of_measurements = self.results_raw[self.esp].shape[0]
        title = "Unprocessed: Scatter plot of measured values"
        xlabel = "Measurement index"
        ylabel = "Measured value [mV]"
        plt.scatter(range(0, number_of_measurements),
                    self.results_raw[self.meter],
                    s=dot_size, facecolors='none', edgecolors='b')
        plt.scatter(range(0, number_of_measurements),
                    self.results_raw[self.esp],
                    s=dot_size, facecolors='none', edgecolors='r')
        plt.title(title, wrap=True)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        self.__save_fig("scatter_raw")

    def create_scatter_comparison_processed(self):
        dot_size = 80
        number_of_measurements = self.results_raw[self.esp].shape[0]
        median_diff_value = round(self.results_processed[
                                      self.diff_col].median())
        title = "Processed: Scatter plot of measured values"
        xlabel = "Measurement index"
        ylabel = "Measured value [mV]"
        plt.scatter(range(0, number_of_measurements),
                    self.results_raw[self.meter],
                    s=dot_size, facecolors='none', edgecolors='b')
        plt.scatter(range(0, number_of_measurements),
                    self.results_raw[self.esp] - median_diff_value,
                    s=dot_size, facecolors='none', edgecolors='r')
        plt.title(title, wrap=True)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        self.__save_fig("scatter_processed")

