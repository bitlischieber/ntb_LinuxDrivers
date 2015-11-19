#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>     // Provides register_chrdev_region, ...
#include <linux/device.h> // Provides device_create, ...
#include <linux/cdev.h>   // Char devices

/* Metainformation
** MODULE_LICENSE is mandatory.
*/
MODULE_AUTHOR("Bitlischieber");
MODULE_DESCRIPTION("Sample driver");
MODULE_LICENSE("GPL");

/* Prototypes
*/
static int deviceOpen(struct inode *, struct file *);
static int deviceClose(struct inode *, struct file *);

/* Class variables
*/
dev_t mDevice;
struct cdev *mCdev;
struct class *mClass;
struct device *mCreatedDevice;

struct file_operations mFops = {
  .owner = THIS_MODULE,
  .open = deviceOpen,
  .release = deviceClose
};

/* Call after load of module.
*/
static int __init init_function(void) {

  int err;

  /***  Register device number ***/
  err = alloc_chrdev_region(&mDevice, 0, 1, "SampleDriver");
  if(err) goto fail_reg;
  printk("SampleDriver registered %u, %u\n", MAJOR(mDevice), MINOR(mDevice));

  /*** Register char device ***/
  mCdev = cdev_alloc();
  mCdev->owner = THIS_MODULE;
  mCdev->ops = &mFops;
  err = cdev_add(mCdev, mDevice, 1);
  if(err) goto fail_cdev_add;

  /*** Create device ***/
  mClass = class_create(THIS_MODULE, "SampleClass");
  mCreatedDevice = device_create(mClass, NULL, mDevice, NULL, "SampleDriver");

  return 0;

  // Failes to register device
  fail_cdev_add:
    printk(KERN_ALERT "Couldn't add the character device\n");
    unregister_chrdev_region(mDevice, 1);
  fail_reg:
    printk(KERN_ALERT "Couldn't register or allocate the device number\n");

    return err;

} // init_function

/* Driver methodes.
*/
static int deviceOpen(struct inode * dev_node, struct file * dev_file) {
  printk(KERN_INFO "deviceOpen called\n");
  return 0;
}

static int deviceClose(struct inode * dev_node, struct file * dev_file) {
  printk(KERN_INFO "deviceClose called\n");
  return 0;
}

/* Call while unload module.
*/
static void __exit exit_function(void) {

  device_destroy(mClass, mDevice);
  class_destroy(mClass);
  cdev_del(mCdev);
  unregister_chrdev_region(mDevice, 1);

  printk(KERN_ALERT "SampleDriver unregistered\n");

} // exit_function

/* Initialisation
** Makro defined methodes for load and unload module.
*/
module_init(init_function);
module_exit(exit_function);
