#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define INTERRUPT_IN 0x81
#define PACKET_SIZE 1

// Module info
MODULE_DESCRIPTION("USB Nintendo Switch Controller Driver");
MODULE_AUTHOR("Alex Lugo <alugocp@aim.com>");
MODULE_LICENSE("GPL");

// Global variables
static struct usb_device *device;
static struct usb_class_driver class;

static int gamepad_open(struct inode *i, struct file *f) {
    return 0;
}

static int gamepad_close(struct inode *i, struct file *f) {
    return 0;
}

static ssize_t gamepad_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off) {
    printk(KERN_INFO "We gotta write...\n");
    return 0;
}

// This function is run whenever we have input to read from the device
static ssize_t gamepad_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    unsigned char data[PACKET_SIZE];
    int read_len;
    int result;
    int a = 0;

    // Reads a data packet from the device
    printk(KERN_INFO "Reading...\n");
    result = usb_interrupt_msg(device, usb_rcvintpipe(device, INTERRUPT_IN), data, PACKET_SIZE, &read_len, 5000);
    if (result) {
        printk(KERN_ERR "Error reading USB gamepad: %d\n", result);
        return result;
    }
    if (copy_to_user(buf, data, MIN(len, read_len))) {
        return -EFAULT;
    }
    printk(KERN_INFO "Gamepad says:");
    while (a < MIN(len, read_len)) {
        printk(KERN_INFO " %d", data[a++]);
    }
    printk(KERN_INFO "\n");
    return MIN(len, read_len);
}

// This table defines the I/O functions
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = gamepad_open,
    .release = gamepad_close,
    .read = gamepad_read,
    .write = gamepad_write,
};

// Probe function that gets called when our device is connected
static int gamepad_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    int result;
    printk(KERN_INFO "Gamepad device detected\n");
    device = interface_to_usbdev(interface);
    class.name = "usb/gamepad%d";
    class.fops = &fops;
    result = usb_register_dev(interface, &class);
    if (result < 0) {
        printk(KERN_ERR "Could not register the USB gamepad\n");
    }
    return result;
}

// Called when our device is disconnected
static void gamepad_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Gamepad device removed\n");
    usb_deregister_dev(interface, &class);
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
