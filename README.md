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
|```cat```|Calls sequential 'open', 'read' and 'close'|
|```scp <file> <user>@<target>:<desitnation>``` | Copy a file over ssh to the target|


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

```printk(KERN_INFO "Assigned major and minor numbers are: %u, %u\n", MAJOR(mDevice), MINOR(mDevice));```

Close the message string using ```\n``` otherwise, the message is throw until a ```\n``` was committed.

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

C error codes: /usr/include/asm-generic/errno-base.h

Formats:
| Data type | Format specifier |
|-----------|------------------|
|int|%d or %x|
|int|%u or %x|
|long|%ld or %lx|
|unsigned long|%lu or %lx|
|long long|%lld or %llx|
|unsigned long long|%llu or %llx|
|size_t|%zu or %zx|
|ssize_t|%zd or %zx|
|s32|%d or %x|
|u32|%u or %x|
|s64|%lld or %llx|
|u64|%llu or %llx|

Pointers:
%pF versatile_init+0x0/0x110
%pf versatile_init
%pS versatile_init+0x0/0x110
%pSR  versatile_init+0x9/0x110 (with __builtin_extract_return_addr() translation)
%ps versatile_init
%pB prev_fn_of_versatile_init+0x88/0x88

Source: http://www.makelinux.net/books/lkd2/ch18lev1sec3

## Device rights

An ```udev``` created device-file belongs to root. To make it accessible by user, the device-file rights can be changed manually using ```chmod```.
Alternatively and more elegant is to define a rule in ```/etc/udev/rules.d/```. The rule filename has two parts: Order-Number and Description text, separated by dash.
The content of the file looks like this:

```KERNEL=="testDevice", OWNDER="root", GROUP"root", "MODE="0666"```

## Makefiles

Make will make files from other files. This can make troubles if you have a file named i.e. <<clean>> and in your makefile also used the label <<clean>>. In this case, make will use the file instead of handle <<clean>> as a label. Using the ```.PHONY``` command will tell make the name isn't associated with a file: ```.PHONY clean```.

## Helpful resources
https://www.kernel.org/doc/htmldocs/

http://lxr.free-electrons.com/source/

http://linux.die.net/man/2
