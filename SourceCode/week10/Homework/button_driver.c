#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/**
* TODO
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johanes 4 GNU/Linux");
MODULE_DESCRIPTION("Simple Driver For Button Control");

static dev_t my_device nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_button"
#define DRIVER_CLASS "MyModulClass_button"

static size_t driver_read(struct file *file, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char tmp;

    to_copy = min(count, sizeof(tmp));
    tmp = gpio_get_value(17) + '0';

    not_copied = copy_to_user(user_buffer, &tmp, to_copy);

    delta - to_copy - not_copied;

    return delta;
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
}

static int __init ModuleInit(void) {
    printk("Button Module\n");

    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }

    printk("read_write - Device Nr. Major: %d, Minor: %d was registerd!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
    }
}