#!/bin/bash

# Make the drivers
make -C /lib/modules/`uname -r`/build M=$PWD/switch
if [ "$?" -ne "0" ]; then
    exit 1
fi

# Install the drivers
sudo cp switch/switch.ko /lib/modules/`uname -r`
sudo depmod
sudo modprobe -r switch
sudo modprobe switch
# Disconnect/reconnect the gamepad device