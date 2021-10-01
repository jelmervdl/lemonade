#!/usr/bin/env python3 
import requests
from argparse import ArgumentParser
import typing
import json
from dataclasses import dataclass
import os
import tarfile

APP = "lemonade"
HOME = os.environ.get("HOME")
CACHE_DIR = os.environ.get("XDG_CACHE_HOME", os.path.join(HOME, ".cache"))
CONFIG_DIR = os.environ.get("XDG_CONFIG_HOME", os.path.join(HOME, ".config"))


@dataclass
class Config:
    url : str = "https://translatelocally.com/models.json"
    cache_dir: str = os.path.join(CACHE_DIR, APP)
    config_dir: str = os.path.join(CONFIG_DIR, APP)
    models_file: str = os.path.join(CONFIG_DIR, APP, "models.json")
    data_dir: str = os.path.join(HOME, ".{}".format(APP))
    archive_dir: str = os.path.join(HOME, ".{}".format(APP), "archives")
    models_dir: str = os.path.join(HOME, ".{}".format(APP), "models")

config = Config()

def create_required_dirs(config):
    os.makedirs(config.cache_dir, exist_ok=True)
    os.makedirs(config.config_dir, exist_ok=True)
    os.makedirs(config.data_dir, exist_ok=True)
    os.makedirs(config.models_dir, exist_ok=True)
    os.makedirs(config.archive_dir, exist_ok=True)

def get_inventory(url, save_location, force_download=False):
    def get_inventory_fresh(url):
        response = requests.get(url)
        return response.text

    def write_inventory_to_file(inventory, save_location):
        with open(save_location, "w+") as models_file:
            models_file.write(inventory)

    if force_download or not os.path.exists(save_location):
        inventory = get_inventory_fresh(url)
        write_inventory_to_file(inventory, save_location)
        return json.loads(inventory)
    else:
        with open(save_location) as models_file:
            data = json.load(models_file)
            return data


def download_archive(url, save_location, force_download=False):
    if force_download or not os.path.exists(save_location):
        response = requests.get(url, stream=True)
        # Throw an error for bad status codes
        response.raise_for_status()
        with open(save_location, 'wb') as handle:
            for block in response.iter_content(1024):
                handle.write(block)




if __name__ == '__main__':
    parser = ArgumentParser("Model manager to download models for bergamot")
    create_required_dirs(config)
    data = get_inventory(config.url, config.models_file)
    for model in data["models"]:
        model_archive ='{}.tar.gz'.format(model["shortName"])
        save_location = os.path.join(config.archive_dir, model_archive)
        download_archive(model["url"], save_location)
        with tarfile.open(save_location) as model_archive:
            model_archive.extractall(config.models_dir)
            print(model)





