# SNESmap USB Gamepad driver
I made a driver for a USB-connected SNES gamepad so I could use it with a Super Mario World emulator on Ubuntu. The prototype is written in Python and the final version is in C.

## Run
You can either run the prototype version (Python3) or the native version in C

#### Prototype
```
sudo python3 prototype.py
```
#### Native
```
make
sudo ./driver
```

## Dependencies
To install all dependencies run this code in your terminal:
```
sudo apt-get install libxdo-dev libusb-1.0-0-dev
pip install pyusb
```
The prototype uses [pyusb](https://github.com/pyusb/pyusb) and the native version uses the following two libraries:
- [libusb](http://libusb.sourceforge.net/api-1.0/libusb_api.html)
- [xdo](https://github.com/jordansissel/xdotool)

## Authors
- **Alex Lugo**

I don't know why I put this section on all my projects because I code solo but hey whatever, subscribe to my [YouTube](https://www.youtube.com/alexlugo) channel I guess
