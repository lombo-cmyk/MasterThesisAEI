import os
from datetime import datetime


def create_directories(upstream_folder_name: str):
    script_dir = os.path.dirname(__file__)
    results_dir = os.path.join(script_dir, upstream_folder_name + '/')
    current_working_dir = os.path.join(results_dir, datetime.now().strftime(
        "%Y_%m_%d_%H_%M_%S/"))
    if not os.path.isdir(results_dir):
        os.makedirs(results_dir)
    os.makedirs(current_working_dir)
    return current_working_dir
