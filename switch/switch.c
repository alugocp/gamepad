/**
 * This code is informed by the following:
 * • https://github.com/torvalds/linux/blob/master/drivers/hid/usbhid/usbmouse.c
 * • https://docs.kernel.org/6.0/driver-api/usb/usb.html
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#define ADDRESS 0x81
#define INTERVAL 8

// Module info
MODULE_DESCRIPTION("USB Nintendo Switch Controller Driver");
MODULE_AUTHOR("Alex Lugo <alugocp@aim.com>");
MODULE_LICENSE("GPL");

// Global variables
static unsigned char gamepad_buffer[8];
static struct usb_device *device;
static struct input_dev *input;
static struct urb *irq;

// Reads some data from the gamepad
static void gamepad_irq(struct urb *urb) {
    unsigned int a;
    switch (urb->status) {
        case 0:
            break;
        case -ECONNRESET:
        case -ESHUTDOWN:
        case -ENOENT:
            return;
        default:
            usb_submit_urb(irq, GFP_ATOMIC);
            return;
    }
    printk(KERN_INFO "Gamepad says:");
    for (a = 0; a < 8; a++) {
        printk(KERN_INFO " 0x%x", gamepad_buffer[a]);
    }
    printk(KERN_INFO "\n");
}

// Called when the gamepad buffer is opened
static int gamepad_open(struct input_dev *dev) {
    return usb_submit_urb(irq, GFP_KERNEL) ? -EIO : 0;
}

// Called when the gamepad buffer is closed
static void gamepad_close(struct input_dev *dev) {
    usb_kill_urb(irq);
}

// Probe function that gets called when our device is connected
static int gamepad_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    int pipe;
    int maxp;
    int result;
    printk(KERN_INFO "Gamepad device detected\n");
    device = interface_to_usbdev(interface);
    irq = usb_alloc_urb(0, GFP_KERNEL);
    if (!irq) {
        printk(KERN_ERR "Could not allocate a URB\n");
    }

    // Set up the input device
    input = input_allocate_device();
    input->name = "usb/switch";
    input->dev.parent = &interface->dev;
    input->open = gamepad_open;
    input->close = gamepad_close;
    irq->dev = device;

    // Initialize logic for gamepad_irq function
    pipe = usb_rcvintpipe(device, ADDRESS);
    maxp = usb_maxpacket(device, pipe, false);
    usb_fill_int_urb(irq, device, pipe, gamepad_buffer, (maxp > 8 ? 8 : maxp), gamepad_irq, NULL, INTERVAL);

    // Register the input device
    result = input_register_device(input);
    if (result) {
        printk(KERN_ERR "Could not register the USB gamepad\n");
    }
    return result;
}

// Called when our device is disconnected
static void gamepad_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Gamepad device removed\n");
    input_unregister_device(input);
}

// Table that lists the vendor/product IDs this driver supports
static struct usb_device_id gamepad_table[] =
{
    { USB_DEVICE(0x20d6, 0xa713) },
    {}
};
MODULE_DEVICE_TABLE (usb, gamepad_table);

// Table that defines probe/disconnect functions
static struct usb_driver gamepad_driver =
{
    .name = "gamepad_driver",
    .id_table = gamepad_table,
    .probe = gamepad_probe,
    .disconnect = gamepad_disconnect,
};

// Initializes the driver (registers a USB driver)
int init_module() {
    return usb_register(&gamepad_driver);
}

// Cleans up the driver code
void cleanup_module() {
    usb_deregister(&gamepad_driver);
}
