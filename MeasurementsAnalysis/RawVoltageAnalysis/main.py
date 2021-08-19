from RawVoltageAnalysis.VoltageAnalyzer import VoltageAnalyzer
from free_functions import create_directories
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("--file", help="Path to parsed file", required=True)
args = parser.parse_args()


def main():
    file_to_parse = None
    if args.file:
        file_to_parse = args.file

    analyzer = VoltageAnalyzer(create_directories("temp_plots"), file_to_parse)
    analyzer.create_raw_values_boxplot()
    analyzer.process_results()
    analyzer.create_processed_values_boxplot()
    analyzer.create_comparison_boxplot()
    analyzer.create_scatter_comparison_raw()
    analyzer.create_scatter_comparison_processed()


if __name__ == '__main__':
    main()
