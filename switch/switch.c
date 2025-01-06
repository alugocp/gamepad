/**
 * This code is informed by the following:
 * • https://github.com/torvalds/linux/blob/master/drivers/hid/usbhid/usbmouse.c
 * • https://docs.kernel.org/6.0/driver-api/usb/usb.html
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#define BUFFER_SIZE 8
#define ADDRESS 0x81
#define INTERVAL 8

// Module info
MODULE_DESCRIPTION("USB Nintendo Switch Controller Driver");
MODULE_AUTHOR("Alex Lugo <alugocp@aim.com>");
MODULE_LICENSE("GPL");

// Global variables
static unsigned char *gamepad_buffer = NULL;
static struct usb_device *device = NULL;
static struct input_dev *idev = NULL;
static struct urb *irq = NULL;
static dma_addr_t data_dma;
static int mouse_down = 0;

// Reads some data from the gamepad
static void gamepad_urb_complete(struct urb *urb) {
    int dx;
    int dy;
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

    // Business logic to handle gamepad buffer data
    if (gamepad_buffer[0] == 4) {
        if (!mouse_down) {
            input_report_key(idev, BTN_LEFT, 1);
            mouse_down = 1;
        }
    } else if (mouse_down) {
        input_report_key(idev, BTN_LEFT, 0);
        mouse_down = 0;
    }
    dx = (int)((gamepad_buffer[3] - 128) / 16);
    dy = (int)((gamepad_buffer[4] - 128) / 16);
    input_report_rel(idev, REL_X, dx);
    input_report_rel(idev, REL_Y, dy);
    input_sync(idev);

    // Resubmit URB so we keep polling
    if (usb_submit_urb(irq, GFP_ATOMIC)) {
        printk(KERN_ERR "Could not resubmit URB\n");
    }
}

// Intelligently frees all the global object pointers
static void gamepad_free_pointers(void) {
    if (irq != NULL) {
        usb_free_urb(irq);
        irq = NULL;
    }
    if (gamepad_buffer != NULL) {
        usb_free_coherent(device, BUFFER_SIZE, gamepad_buffer, data_dma);
        gamepad_buffer = NULL;
    }
    if (idev != NULL) {
        input_free_device(idev);
        idev = NULL;
    }
}

// Probe function that gets called when our device is connected
static int gamepad_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    int pipe;
    int result;
    printk(KERN_INFO "Gamepad device detected\n");

    // Allocate and register the input device (controls the mouse)
    idev = input_allocate_device();
    idev->name = "gamepad_mouse";
    set_bit(EV_KEY, idev->evbit);
    set_bit(BTN_LEFT, idev->keybit);
    set_bit(EV_REL, idev->evbit);
    set_bit(REL_X, idev->relbit);
    set_bit(REL_Y, idev->relbit);
    result = input_register_device(idev);
    if (result) {
        printk(KERN_ERR "Could not register input device\n");
        gamepad_free_pointers();
        return result;
    }

    // Initialize logic to capture USB input stream data
    device = interface_to_usbdev(interface);
    irq = usb_alloc_urb(0, GFP_KERNEL);
    if (!irq) {
        printk(KERN_ERR "Could not allocate a URB\n");
        gamepad_free_pointers();
        return -ENOENT;
    }
    pipe = usb_rcvintpipe(device, ADDRESS);
    gamepad_buffer = usb_alloc_coherent(device, BUFFER_SIZE, GFP_KERNEL, &data_dma);
    if (!gamepad_buffer) {
        printk(KERN_ERR "Could not allocate buffer space\n");
        gamepad_free_pointers();
        return -ENOENT;
    }
    usb_fill_int_urb(irq, device, pipe, gamepad_buffer, BUFFER_SIZE, gamepad_urb_complete, NULL, INTERVAL);

    // Kick off the URB submission cycle
    result = usb_submit_urb(irq, GFP_KERNEL);
    if (result) {
        printk(KERN_ERR "Could not submit URB\n");
        gamepad_free_pointers();
    }
    return result;
}

// Called when our device is disconnected
static void gamepad_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Gamepad device removed\n");
    input_unregister_device(idev);
    gamepad_free_pointers();
}

// Table that lists the vendor/product IDs this driver supports
static struct usb_device_id gamepad_table[] = {
    { USB_DEVICE(0x20d6, 0xa713) },
    {}
};
MODULE_DEVICE_TABLE (usb, gamepad_table);

// Table that defines probe/disconnect functions
static struct usb_driver gamepad_driver = {
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
    gamepad_free_pointers();
}
