#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>   // Provides MKDEV
#include <linux/fs.h>       // Provides register_chrdev_region, ...
#include <linux/device.h>   // Provides device_create, ...
#include <linux/cdev.h>     // Char devices
#include <linux/uaccess.h>  // Access to data from userspace
#include <linux/ioctl.h>
#include <linux/time.h>
#include <asm/io.h>
#include <asm/uaccess.h>
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
static long deviceIoctl(struct file *, unsigned int, unsigned long);
void setLED(u8);

/* Class variables
*/
dev_t mDevice;
struct cdev *mCdev;
struct class *mClass;
u32 mResCnt = 0;

struct file_operations mFops = {
  .owner = THIS_MODULE,
  .read = deviceRead,
  .write = deviceWrite,
  .open = deviceOpen,
  .unlocked_ioctl = deviceIoctl,
  .release = deviceClose
};

typedef struct {
  enum TIMER_FUNCTION func;
  u8 armed;
  struct timeval start;
  struct timeval stop;
} func_timer;

func_timer mTimer;

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

/* Return the switch state
*/
int deviceRead(struct file *dev_file, char __user *data, size_t size, loff_t *offs){

  u32 temp;
  char val;

  static unsigned int *pinctrlBasePrt;

  pinctrlBasePrt = ioremap(GPIO_REG_BASE, GPIO_REG_SIZE);

  temp = ioread32(pinctrlBasePrt + GPIO_DATA_IN_OFFSET);
  val = (char)(temp & ((1 << GPIO0_0) | (1 << GPIO0_1)) & 0x03);  // Mask the tact switches

  printk(KERN_INFO "Switch state read %x\n", (val ^ 0x03));

  // invert the tact switches (lines are pulled up)
  if(put_user((val ^ 0x03), data)) {
    return 0;
  } else {
    return -EINVAL;
  } // if put_user
}

/* Switch the LEDs
*/
int deviceWrite(struct file *dev_file, const char __user *data, size_t size, loff_t *offs) {

  u8 val;

  // Get to be switched LEDs from userspace
  if (!get_user(val,data)) {
    // Data received
    setLED(val);
    printk(KERN_INFO "LED state written %x\n", val);

    return 1;
  } else {
    // No data received
    printk(KERN_INFO "No LED date received");
    return -EINVAL;
  } // if

}

void setLED(u8 value) {

  u32 temp;
  static unsigned int *pinctrlBasePrt;

  pinctrlBasePrt = ioremap(GPIO_REG_BASE, GPIO_REG_SIZE);

  temp = ioread32(pinctrlBasePrt + GPIO_DATA_OUT_OFFSET);
  // clear all fields
  temp &= ~((1 << GPIO0_2) | (1 << GPIO0_6) | (1 << GPIO0_13) | (1 << GPIO0_15));
  // set fields
  if(value & 0x01) temp |= (1 << GPIO0_2);
  if(value & 0x02) temp |= (1 << GPIO0_6);
  if(value & 0x04) temp |= (1 << GPIO0_13);
  if(value & 0x08) temp |= (1 << GPIO0_15);
  iowrite32(temp, pinctrlBasePrt + GPIO_DATA_OUT_OFFSET);
  mResCnt++;

} // setLED

long deviceIoctl(struct file *dev_file, unsigned int cmd, unsigned long arg) {

  _stopWatchStruct swStruct;
  u32 temp;
  u8 val;
  int error = 0;
  _ledStruct ledStruct;

  static unsigned int *pinctrlBasePrt;

  pinctrlBasePrt = ioremap(GPIO_REG_BASE, GPIO_REG_SIZE);

  switch (cmd) {
    case READ_LEDS:
      if(mTimer.armed && mTimer.func == TFUNC_READ_LEDS) do_gettimeofday(&mTimer.start);
      temp = ioread32(pinctrlBasePrt + GPIO_DATA_OUT_OFFSET);
      // Rearange the bits to the lower nibble
      temp = ((temp >> GPIO0_2) | (temp >> GPIO0_6) | (temp >> GPIO0_13) | (temp >> GPIO0_15));
      val = (u8)(temp & 0x0f);
      printk(KERN_INFO "LED state: %x\n", val);
      if(mTimer.armed && mTimer.func == TFUNC_READ_LEDS) do_gettimeofday(&mTimer.stop);
      break;

    case WRITE_LEDS:
      if(mTimer.armed && mTimer.func == TFUNC_WRITE_LEDS) do_gettimeofday(&mTimer.start);
      // Read current state
      temp = ioread32(pinctrlBasePrt + GPIO_DATA_OUT_OFFSET);
      // Read data from user space
      error = copy_from_user(&ledStruct, (void __user *)arg, sizeof(ledStruct));
      if(error != 0) {
        printk(KERN_ALERT "Couldn't copy data from userspace!");
        return -EINVAL;
      }
      // set LED (I know, there are much simpler ways to do that, but I liked to try to use a structure)
      if(ledStruct.LED_No == 0) {
        temp &= ~(1 << GPIO0_2);
        if(ledStruct.LED_State) temp |= (1 << GPIO0_2);
      } // LED 0
      if(ledStruct.LED_No == 1) {
        temp &= ~(1 << GPIO0_6);
        if(ledStruct.LED_State) temp |= (1 << GPIO0_6);
      } // LED 1
      if(ledStruct.LED_No == 2) {
        temp &= ~(1 << GPIO0_13);
        if(ledStruct.LED_State) temp |= (1 << GPIO0_13);
      } // LED 2
      if(ledStruct.LED_No == 3) {
        temp &= ~(1 << GPIO0_15);
        if(ledStruct.LED_State) temp |= (1 << GPIO0_15);
      } // LED 3
      iowrite32(temp, pinctrlBasePrt + GPIO_DATA_OUT_OFFSET);
      mResCnt++;
      printk(KERN_INFO "LED state set!");
      if(mTimer.armed && mTimer.func == TFUNC_WRITE_LEDS) do_gettimeofday(&mTimer.stop);
      break;

    case ALL_LEDS_ON:
      if(mTimer.armed && mTimer.func == TFUNC_ALL_LEDS_ON) do_gettimeofday(&mTimer.start);
      setLED(0x0f);
      if(mTimer.armed && mTimer.func == TFUNC_ALL_LEDS_ON) do_gettimeofday(&mTimer.stop);
      break;

    case ALL_LEDS_OFF:
      if(mTimer.armed && mTimer.func == TFUNC_ALL_LEDS_OFF) do_gettimeofday(&mTimer.start);
      setLED(0x00);
      if(mTimer.armed && mTimer.func == TFUNC_ALL_LEDS_OFF) do_gettimeofday(&mTimer.stop);
      break;

    case APPLY_PATTERN:
      if(mTimer.armed && mTimer.func == TFUNC_APPLY_PATTERN) do_gettimeofday(&mTimer.start);
      error = copy_from_user(&val, (void __user *)arg, sizeof(val));
      if(error != 0) {
        printk(KERN_ALERT "Couldn't read pattern!");
        return -EINVAL;
      }
      setLED(val);
      if(mTimer.armed && mTimer.func == TFUNC_APPLY_PATTERN) do_gettimeofday(&mTimer.stop);
      break;

    case GET_NOF_RES:
      if(mTimer.armed && mTimer.func == TFUNC_GET_NOF_RES) do_gettimeofday(&mTimer.start);
      error = copy_to_user((void __user *)arg, &mResCnt, sizeof(mResCnt));
      if(error != 0) {
        printk(KERN_DEBUG "Couldn't copy to userspace!");
        return -1;
      }
      if(mTimer.armed && mTimer.func == TFUNC_GET_NOF_RES) do_gettimeofday(&mTimer.stop);
      break;

    case READ_BUTTONS:
      if(mTimer.armed && mTimer.func == TFUNC_READ_BUTTONS) do_gettimeofday(&mTimer.start);
      temp = ioread32(pinctrlBasePrt + GPIO_DATA_IN_OFFSET);
      val = (u8)(temp & 0x03);
      error = copy_to_user((void __user *)arg, &val, sizeof(val));
      if(error != 0) {
        printk(KERN_DEBUG "Couldn't copy to userspace!");
        return -1;
      }
      printk(KERN_INFO "Tact switch state: %x\n", val);
      if(mTimer.armed && mTimer.func == TFUNC_READ_BUTTONS) do_gettimeofday(&mTimer.stop);
      break;

    case INIT_TIMER:
      error = copy_from_user(&swStruct, (void __user *)arg, sizeof(swStruct));
      if(error != 0) {
        printk(KERN_ALERT "Couldn't read timer function!");
        return -EINVAL;
      }

      if (mTimer.armed) {
        // evaluate running timer
        if (mTimer.func != swStruct.func) {
          printk(KERN_ALERT "Timer already running with other function!");
          return -EINVAL;
        }
        swStruct.duration_us = (mTimer.stop.tv_usec - mTimer.start.tv_usec);
        mTimer.armed = 0;
        error = copy_to_user((void __user *)arg, &swStruct, sizeof(swStruct));
        if(error != 0) {
          printk(KERN_DEBUG "Couldn't copy to userspace!");
          return -1;
        }
      } else {
        // Start new measure
        mTimer.func = swStruct.func;
        mTimer.armed = 1;
      }
      break;

    default:
      printk(KERN_ALERT "Unknown command: %d\n", cmd);
      return -EINVAL;
      break;
  } // switch

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
