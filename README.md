# Linux Driver Development
Exercises for NTB walk through "Einstieg in die Linux Treiberentwicklung".

## Useful console commands

|Command | Description |
|--------|-------------|
|```pwd```| Returns the current path |
|```uname -r```| Returns the version of the Kernel headers|
|```insmod <module>.ko```|Attach module to Kernel|
|```rmmod <module>.ko```|Remove module from Kernel|
|```dmesg```|Prints the Kernel log|
|```tail```|Returns the last 10 lines of a file (usage i.e. ```dmesg | tail```)|

## Kernel development restrictions
- Driver can be interrupted at any time. Make sure, the usage of shared resources doesn't lead to any troubles.
- The Kernel owns just a small Stack, use it with care. Don't use recursive methods.
- Floating point operations are not possible.
- Use double underlined "__" function calls with care. They are low level calls.
- No access to the common used C libraries. However, the Kernel provides its own library containing the most important functions.

## Command hints
### Allocation and registration
```#include <linux/fs.h>``` is needed by ```register_chrdev_region```, ```alloc_chrdev_region``` and ```unregister_chrdev_region```.
```#include <linux/kdev_t.h>``` is needed by ```MKDEV(...,...)```, ```MINOR(...)``` and ```MAJOR(...)```.

### Printout
Formated Kernel print message:
```printk(KERN_INFO "Assigned major and minor numbers are: %u, %u", MAJOR(mDevice), MINOR(mDevice));```

| Loglevel | Description |
|----------|-------------|
|KERN_EMERG|An emergency condition; the system is probably dead|
|KERN_ALERT|A problem that requires immediate attention|
|KERN_CRIT|A critical condition|
|KERN_ERR|An error|
|KERN_WARNING|A warning|
|KERN_NOTICE|A normal, but perhaps noteworthy, condition|
|KERN_INFO|An informational message|
|KERN_DEBUG|A debug message typically superfluous|

Source: http://www.makelinux.net/books/lkd2/ch18lev1sec3
