#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

static int upper_bound = -1;

static ssize_t azar_read(struct file *filp, char __user *data, size_t size, loff_t *offset) {
    /* Completar */
    printk("%d" , upper_bound);
    if (upper_bound <= 0) {
        return -EPERM;
    }
    
    unsigned int random_number;
    char* intToStringBuffer;
    int minimum;

    /*if (size < sizeof(random_number)) {
        return -EINVAL;
    }*/

    intToStringBuffer = kmalloc(size, GFP_KERNEL);

    get_random_bytes(&random_number, sizeof(random_number));
    random_number = random_number % (upper_bound);
    printk("el random number es: %d" , random_number);

    minimum = snprintf(intToStringBuffer, size, "%d\n", random_number);
    minimum = min(minimum, size);


    printk("nuestro string es: %s" , intToStringBuffer);

    copy_to_user(data, intToStringBuffer, minimum);

    kfree(intToStringBuffer);

    return minimum;
}

static ssize_t azar_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {
    /* Completar */
    int result;
    char* intAsStringBuffer;
    
    intAsStringBuffer = kmalloc(size + 1, GFP_KERNEL);
    copy_from_user(intAsStringBuffer, data, size);
    intAsStringBuffer[size] = '\0';
    if (kstrtoint(intAsStringBuffer, 10, &result) != 0) {
        kfree(intAsStringBuffer);
        return -EPERM;
    }
    upper_bound = result;

    kfree(intAsStringBuffer);
    
    return size;
}

static struct file_operations azar_fops = {
  .owner = THIS_MODULE,
  .read = azar_read,
  .write = azar_write,
};

static dev_t azar_devno;
static struct cdev azar_cdev;
static struct class *azar_class;

static int __init azar_init(void) {
    cdev_init(&azar_cdev, &azar_fops);
    alloc_chrdev_region(&azar_devno, 0, 1, "SO-azar");
    cdev_add(&azar_cdev, azar_devno, 1);

    azar_class = class_create(THIS_MODULE, "azar");
    device_create(azar_class, NULL, azar_devno, NULL, "azar");

    printk(KERN_ALERT "Loading module 'azar'\n");
    return 0;
}

static void __exit azar_exit(void) {
    printk(KERN_ALERT "Unloading module 'azar'\n");

    device_destroy(azar_class, azar_devno);
    class_destroy(azar_class);

    unregister_chrdev_region(azar_devno, 1);
    cdev_del(&azar_cdev);
}

module_init(azar_init);
module_exit(azar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al azar");
