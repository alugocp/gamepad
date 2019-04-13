from sys import argv
import subprocess
import usb.core

dev=usb.core.find(idVendor=0x0079,idProduct=0x0011)
if not dev:
    print("Device not connected")
    exit()

ep=dev[0].interfaces()[0].endpoints()[0]
i=dev[0].interfaces()[0].bInterfaceNumber

dev.reset()
if dev.is_kernel_driver_active(i):
    dev.detach_kernel_driver(i)
dev.set_configuration()

def buttons(data):
    presses=[]
    data5=[["Y",128],["B",64],["A",32],["X",16]]
    d=data[5]-15
    for k in data5:
        if d>=k[1]:
            d-=k[1]
            presses.append(k[0])
    if data[3]==255: presses.append("RIGHT")
    if data[3]==0: presses.append("LEFT")
    if data[4]==0: presses.append("UP")
    if data[4]==255: presses.append("DOWN")
    data6=[["START",32],["SELECT",16],["R",4],["L",1]]
    d=data[6]
    for k in data6:
        if d>=k[1]:
            d-=k[1]
            presses.append(k[0])
    return presses

controls={
    "A":"v",
    "B":"c",
    "X":"d",
    "Y":"x",
    "UP":"Up",
    "DOWN":"Down",
    "LEFT":"Left",
    "RIGHT":"Right",
    "SELECT":"Return",
    "START":"space",
    "R":"s",
    "L":"a"
}
last=[]
eaddr=(ep.bEndpointAddress)
r=dev.read(eaddr,96)
print("SNESmap is online")
if len(argv)>1 and argv[1]=="test":
    print("Test mode activated")
    last=None
    while True:
        r=dev.read(eaddr,8)
        if len(r)>0 and r!=last:
            print(r)
            last=r

while True:
    r=dev.read(eaddr,8)
    if len(r)>0:
        presses=buttons(r)
        for k in last:
            if not k in presses:
                subprocess.call(["xdotool","keyup",controls[k]])
        for k in presses:
            if not k in last:
                subprocess.call(["xdotool","keydown",controls[k]])
        last=presses
