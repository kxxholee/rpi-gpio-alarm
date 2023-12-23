#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/**     [SEGMENT DRIVER]
*   TODO:
*   1. driver_write
*   2. driver_open
*   3. driver_close
*   4. __init ModuleInit
*   5. __exit ModuleExit
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Simple driver for Led");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_led"
#define DRIVER_CLASS "MyModuleClass_led"

/**
* @brief Write Data to buffer
*/

unsigned char led_hist = '0';
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    unsigned char value = 0;

    to_copy = min(count, sizeof(value));
    not_copied = copy_from_user(&value, user_buffer, to_copy);

    /* Setting the segments LED */
    
    if (value == 'd') {
        gpio_set_value(22, 0);
    }
    else if (value == 'u') {
        gpio_set_value(22, 1);
    }
    else {
        printk("Invalid device input!");
    }

    /* Calculate data */
    delta = to_copy - not_copied;
    return delta;
}

/**
* @brief This function is called, when the device file is opened 
*/
static int driver_open(struct inode *device_file, struct file *instance) {
    printk("led - open was called!\n");
    return 0;
}

/**
* @brief This function is called, when the dvice file is opened
*/
static int driver_close(struct inode *device_file, struct file *instance) {
    printk("led - close was called!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    // .read = driver_read,
    .write = driver_write
};

/**
* @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) {
    printk("Led Module: Hello!\n");

    /* Allocate a device nr */
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    /* Create device class */
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Can not create device file!\n");
        goto FileError;
    }

    /* Initialize device file */
    cdev_init(&my_device, &fops);

    /* Registering devic to kernel */
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }

    if (gpio_request(22, "rpi-gpio-22")) {
        printk("Can not allocate GPIO 22\n");
        goto AddError;
    }

    if (gpio_direction_output(22, 0)) {
        printk("Can not set GPIO 22 to output!\n");
        goto Gpio22Error;
    }

    return 0;

Gpio22Error:
    gpio_free(22);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}

/**
* @brief This function is called, when the module is removed from the kernel
*/
static void __exit ModuleExit(void) {
    gpio_set_value(22, 0);
    gpio_free(22);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Segment Module: Bye Bye\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
