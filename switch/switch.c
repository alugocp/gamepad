#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
MODULE_DESCRIPTION("USB Nintendo Switch Controller Driver");
MODULE_AUTHOR("Alex Lugo <alugocp@aim.com>");
MODULE_LICENSE("GPL");

// Probe function that gets called when our device is connected
static int gamepad_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "Gamepad driver plugged\n");
    return 0;
}

// Called when our device is disconnected
static void gamepad_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Gamepad driver removed\n");
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
