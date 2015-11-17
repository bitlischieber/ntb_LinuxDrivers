#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h> // Provides MKDEV
#include <linux/fs.h> // Provices register_chrdev_region, ...

/* Metainformation
** MODULE_LICENSE is mandatory.
*/
MODULE_AUTHOR("Bitlischieber");
MODULE_DESCRIPTION("Device number allocation module");
MODULE_LICENSE("GPL");

/* Switch for static or dynamic major number allocation
*/
#define MAJOR_ALLOCATION_STATIC 1

/* Class variables
*/
dev_t mDevice;

/* Call after load of module.
*/
static int __init init_function(void) {

  int err;

  if (MAJOR_ALLOCATION_STATIC) {
      mDevice = MKDEV(50,0);
      err = register_chrdev_region(mDevice, 2, "Dummy device");
      if(err) goto fail_reg;
  }else{
      err = alloc_chrdev_region(&mDevice, 0, 2, "Dummy device");
      if(err) goto fail_reg;
  } // if MAJOR_ALLOCATION_STATIC

  printk(KERN_INFO "Dummy device registered %u, %u", MAJOR(mDevice), MINOR(mDevice));
  return 0;

  // Failes to register device
  fail_reg:
    printk(KERN_ALERT "Couldn't register or allocate the device number\n");
    return err;

} // init_function

/* Call while unload module.
*/
static void __exit exit_function(void) {
  unregister_chrdev_region(mDevice, 2);
  printk(KERN_ALERT "Dummy device unregistered\n");
} // exit_function

/* Initialissation
** Makro defined methodes for load and unload module.
*/
module_init(init_function);
module_exit(exit_function);
