#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>

static ssize_t nulo_read(struct file *filp, char __user *data, size_t s, loff_t *off) {
  return 0;
}

static ssize_t nulo_write(struct file *filp, const char __user *data, size_t s, loff_t *off) {
  //completar
  return s;
}

static struct file_operations nulo_operaciones = {
  .owner = THIS_MODULE,
  // Completar
  .read = nulo_read,
  .write = nulo_write
};

static struct cdev nulo_device;
static dev_t major;
static struct class *nulo_class;

static int __init hello_init(void) {
  /* Completar */
  cdev_init(&nulo_device, &nulo_operaciones);
  alloc_chrdev_region(&major, 0, 1, "nulo");
  cdev_add(&nulo_device, major, 1);
  nulo_class = class_create(THIS_MODULE, "nulo");
  device_create(nulo_class, NULL, major, NULL, "nulo");
	
  return 0;
}

static void __exit hello_exit(void) {
  /* Completar */
  unregister_chrdev_region(major, 1);
  cdev_del(&nulo_device);
  device_destroy(nulo_class, major);
  class_destroy(nulo_class);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de '/dev/null'");
