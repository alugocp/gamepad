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

## Notes
- [Linux USB drivers guide](https://sysplay.github.io/books/LinuxDrivers/book/Content/Part11.html)
- [Definition of USB_DEVICE macro](https://github.com/torvalds/linux/blob/63676eefb7a026d04b51dcb7aaf54f358517a2ec/include/linux/usb.h#L986)
