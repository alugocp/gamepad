# SNESmap USB Gamepad driver
This is a collection of drivers for using Nintendo USB controllers on Linux machines.

## Dependencies
To install all dependencies run this code in your terminal:

```bash
# Installs driver dependencies
sudo apt-get install libxdo-dev libusb-1.0-0-dev

# Installs reverse engineering tool dependencies
sudo apt-get install python3-pip
sudo python3 -m pip install pyusb
```

This will install the following two libraries:
- [libusb](http://libusb.sourceforge.net/api-1.0/libusb_api.html) allows the drivers to communicate with USB devices
- [xdo](https://github.com/jordansissel/xdotool) allows the drivers to mock user input
- [pyusb](https://pypi.org/project/pyusb/) for the reverse engineering tool

## Install
Run the following command after downloading this repository:

```bash
./install.sh
```

## Reverse engineering
This repository contains a tool to help you reverse engineer controller protocols.
To activate it simply type the following:

```bash
sudo python3 reverse.py
```

## Authors
- **Alex Lugo**
