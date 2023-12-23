#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/**  [BUTTON DRIVER]
*   TODO:
*   1. driver_read
*   2. driver_open
*   3. driver_close
*   4. __init ModuleInit
*   5. __exit ModuleExit
*/

/**
*   PINS:      [IN] 5, 6
*   BUFBITS:        0, 1 (2^idx)
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johanes 4 GNU/Linux");
MODULE_DESCRIPTION("Simple Driver For Button Control");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_button"
#define DRIVER_CLASS "MyModulClass_button"

unsigned char HistoryD = '0';
unsigned char HistoryU = '0'; // Recording the History of button state
unsigned char ButtonUFlag = '0';
unsigned char ButtonDFlag = '0';
static ssize_t driver_read(struct file *file, char *user_buffer, size_t count, loff_t *offs) {
    int not_copied;
    int FlagSize = min(count, sizeof(unsigned char));

    ButtonUFlag = (gpio_get_value(5) == 1) ? 'u' : '0'; // 'u' flag
    ButtonDFlag = (gpio_get_value(6) == 1) ? 'd' : '0'; // 'd' flag

    if ((ButtonUFlag == 'u') && (ButtonUFlag != HistoryU)) not_copied = copy_to_user(user_buffer, &ButtonUFlag, FlagSize);
    if ((ButtonDFlag == 'd') && (ButtonDFlag != HistoryD)) not_copied = copy_to_user(user_buffer, &ButtonDFlag, FlagSize);
    HistoryU = ButtonUFlag; // Update UpButton History
    HistoryD = ButtonDFlag; // Update DownButton History
    ButtonDFlag = '0';
    ButtonUFlag = '0';

    return FlagSize - not_copied; // return delta
}

static int driver_open(struct inode *device_file, struct file *instance) {
    printk("Button - Open was called!\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
    printk("Button - Close was called!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read
    // .write = driver_write
};

static int __init ModuleInit(void) {
    printk("Button Module: Hello!\n");

    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }

    printk("read_write - Device Nr. Major: %d, Minor: %d was registerd!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Connot create device file!\n");
        goto FileError;
    }

    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }

    // GPIO 5 Exception Handling
    if (gpio_request(5, "rpi-gpio-5")) {
        printk("Can not allocate GPIO 5\n");
        goto AddError;
    }

    if (gpio_direction_input(5)) {
        printk("Can not set GPIO 5 to input!\n");
        goto Gpio5Error;
    }

    // GPIO 6 Exception Handling
    if (gpio_request(6, "rpi-gpio-6")) {
        printk("Can not allocate GPIO 6\n");
        goto AddError;
    }

    if (gpio_direction_input(6)) {
        printk("Can not set GPIO 6 to input!\n");
        goto Gpio6Error;
    }

    return 0; // end of __init ModuleInit()

    // Start Exception Handling
Gpio5Error:
    gpio_free(5);
Gpio6Error:
    gpio_free(6);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);

    return -1; // Return Error Value
}

static void __exit ModuleExit(void) {
    // Input -> No Need to set value to 0
    gpio_free(5);
    gpio_free(6);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Button Module: Bye Bye\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
