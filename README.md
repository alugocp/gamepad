# SNESmap USB Gamepad driver
This is a collection of drivers for using Nintendo USB controllers on Linux machines.

## Dependencies
To install all dependencies run this code in your terminal:

```bash
sudo apt-get install libxdo-dev libusb-1.0-0-dev
```

This will install the following two libraries:
- [libusb](http://libusb.sourceforge.net/api-1.0/libusb_api.html)
- [xdo](https://github.com/jordansissel/xdotool)

## Install
Run the following command after downloading this repository:

```bash
make drivers install
```

## Authors
- **Alex Lugo**
