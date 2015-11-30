#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>   // Provides MKDEV
#include <linux/fs.h>       // Provides register_chrdev_region, ...
#include <linux/device.h>   // Provides device_create, ...
#include <linux/cdev.h>     // Char devices
#include <linux/uaccess.h>  // Access to data from userspace
#include <asm/io.h>
#include <asm/types.h>
#include "omapl138exp_io.h"

/* Metainformation
** MODULE_LICENSE is mandatory.
*/
MODULE_AUTHOR("Bitlischieber");
MODULE_DESCRIPTION("zoom Omap io library");
MODULE_LICENSE("GPL");

/* Prototypes
*/
static int deviceOpen(struct inode *, struct file *);
static int deviceClose(struct inode *, struct file *);
static int deviceRead(struct file *, char __user *, size_t, loff_t *);
static int deviceWrite(struct file *, const char __user *, size_t, loff_t *);
/* Class variables
*/
dev_t mDevice;
struct cdev *mCdev;
struct class *mClass;

struct file_operations mFops = {
  .owner = THIS_MODULE,
  .open = deviceOpen,
  .release = deviceClose
};

/* Other methodes of module.
*/
int deviceOpen(struct inode * dev_node, struct file * dev_file) {

  u32 temp;
  static unsigned int *pinmuxBasePtr;
  static unsigned int *pinctrlBasePrt;

  printk(KERN_INFO "Configure GPIOs\n");

  pinmuxBasePtr = ioremap(PINMUX_REG_BASE, PINMUX_REG_SIZE);
  pinctrlBasePrt = ioremap(GPIO_REG_BASE, GPIO_REG_SIZE);

  /*** Configure GPIOs ***/
  temp = ioread32(pinmuxBasePtr + PINMUX1_OFFSET);
  temp |= 0x88800080; // Set GP0[0], GP0[1], GP0[2], GP0[6] as GPIO
  temp &= 0x888fff8f;
  iowrite32(temp, pinmuxBasePtr + PINMUX1_OFFSET);

  temp = ioread32(pinmuxBasePtr + PINMUX0_OFFSET);
  temp |= 0x00000808; // Set GP0[13], GP0[15] as GPIO
  temp &= 0xfffff8f8;
  iowrite32(temp, pinmuxBasePtr + PINMUX0_OFFSET);

  /*  GPIO BANK01:
  **
  **    15     14     13     12     11     10     9      8      7      6      5      4      3      2      1      0
  **  GP0P15 GP0P14 GP0P13 GP0P12 GP0P11 GP0P10 GP0P9  GP0P8  GP0P7  GP0P6  GP0P5  GP0P4  GP0P3  GP0P2  GP0P1  GP0P0
  **   Out           Out                                              Out                         Out    In     In
  ** |                           |                           |                           |                          |
  */

  /*** Configure DDR ***/
  temp = ioread32(pinctrlBasePrt + GPIO_DDR_OFFSET);
  temp &= ~((1 << GPIO0_2) | (1 << GPIO0_6) | (1 << GPIO0_13) | (1 << GPIO0_15));
  iowrite32(temp, pinctrlBasePrt + GPIO_DDR_OFFSET);

  return 0;
}

int deviceClose(struct inode * dev_node, struct file * dev_file) {

    return 0;
}

/* Call after load of module.
*/
static int zoom_Omap_io_init(void) {

  int err;

  printk(KERN_INFO "Init zoom Omap driver\n");

  /***  Register device number ***/
  err = alloc_chrdev_region(&mDevice, 0, 1, "omapgpio");
  if(err) goto fail_reg;

  /*** Register char device ***/
  mCdev = cdev_alloc();
  mCdev->owner = THIS_MODULE;
  mCdev->ops = &mFops;
  err = cdev_add(mCdev, mDevice,1);
  if(err) goto fail_cdev_add;

  /*** Create device ***/
  mClass = class_create(THIS_MODULE, "omapgpioClass");
  device_create(mClass, NULL, mDevice, NULL, "omapgpio");

  return 0;

  // Failes to register device
  fail_cdev_add:
    printk(KERN_ALERT "Couldn't add the character device\n");
    unregister_chrdev_region(mDevice, 1);
  fail_reg:
    printk(KERN_ALERT "Couldn't register or allocate the device number\n");

    return err;
}

/* Call while unload module.
*/
static void zoom_Omap_io_exit(void) {

  device_destroy(mClass, mDevice);
  class_destroy(mClass);
  cdev_del(mCdev);
  unregister_chrdev_region(mDevice, 1);

  printk(KERN_INFO "zoom Omap GPIO driver unloaded\n");

}

/* Initialisation
** Makro defined methodes for load and unload module.
*/
module_init(zoom_Omap_io_init);
module_exit(zoom_Omap_io_exit);
