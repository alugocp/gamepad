#!/bin/bash

# Make the drivers
make -C /lib/modules/`uname -r`/build M=$PWD/switch

# Install the drivers
sudo cp switch/switch.ko /lib/modules/`uname -r`
sudo depmod
sudo modprobe -r switch
sudo modprobe -r usbhid
sudo modprobe switch
sudo modprobe usbhid
# Disconnect/reconnect the gamepad device