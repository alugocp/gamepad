import subprocess
import usb.core
import sys

# Find the requested USB device
dev = usb.core.find(idVendor = 0x20d6, idProduct = 0xa713)
if not dev:
    print("Device not connected")
    exit()

# Retrieve the right interface/endpoint data
i = dev[0].interfaces()[0].bInterfaceNumber
ep = dev[0].interfaces()[0].endpoints()[0]
if ep.bEndpointAddress == 0x02:
    ep = dev[0].interfaces()[0].endpoints()[1]

# Connect to the device
dev.reset()
if dev.is_kernel_driver_active(i):
    dev.detach_kernel_driver(i)
dev.set_configuration()

# Print whatever we read from the device
last = []
eaddr = (ep.bEndpointAddress)
print("Reverse engineering tool is online")
last = None
while True:
    r = dev.read(eaddr, 8)
    if len(r) > 0 and r != last:
        print(", ".join(list(map(str, r))))
        last = r
