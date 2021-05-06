import os
from datetime import datetime
from VoltageAnalyzer import VoltageAnalyzer
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("--file", help="Path to parsed file", required=True)
args = parser.parse_args()


def create_directories():
    script_dir = os.path.dirname(__file__)
    results_dir = os.path.join(script_dir, 'temp_plots/')
    current_working_dir = os.path.join(results_dir, datetime.now().strftime(
        "%Y_%m_%d_%H_%M_%S/"))
    if not os.path.isdir(results_dir):
        os.makedirs(results_dir)
    os.makedirs(current_working_dir)
    return current_working_dir


def main():
    file_to_parse = None
    if args.file:
        file_to_parse = args.file

    analyzer = VoltageAnalyzer(create_directories(), file_to_parse)
    analyzer.create_raw_values_boxplot()
    analyzer.process_results()
    analyzer.create_processed_values_boxplot()
    analyzer.create_comparison_boxplot()
    analyzer.create_scatter_comparison_raw()
    analyzer.create_scatter_comparison_processed()


if __name__ == '__main__':
    main()
