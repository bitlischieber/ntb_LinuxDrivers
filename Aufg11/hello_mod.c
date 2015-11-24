#include <linux/init.h>
#include <linux/module.h>

/* Metainformation
** MODULE_LICENSE is mandatory.
*/
MODULE_AUTHOR("Bitlischieber");
MODULE_DESCRIPTION("Hello world module");
MODULE_LICENSE("GPL");

/* Call after load of module.
*/
static int hello_init(void) {
  printk(KERN_ALERT "Hello world\n");
  return 0;
}

/* Call while unload module.
*/
static void hello_exit(void) {
  printk(KERN_ALERT "Goodbye, cruel world\n");
}

/* Initialisation
** Makro defined methodes for load and unload module.
*/
module_init(hello_init);
module_exit(hello_exit);
