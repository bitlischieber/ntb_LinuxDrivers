#ifndef OMAPL138Exp_IO_H_
#define OMAPL138Exp_IO_H_

// Pinmux Register Definitions
#define PINMUX_REG_BASE  0x01C14120
#define PINMUX_REG_SIZE  0x0C
#define PINMUX0_OFFSET  0
#define PINMUX1_OFFSET  1
#define PINMUX2_OFFSET  2

// GPIO Register Definitions
#define GPIO_MEM_BASE  0x01E26000
#define GPIO_BANK01_BASE  (GPIO_MEM_BASE + 0x10)
#define GPIO_REG_BASE  GPIO_BANK01_BASE  // GPIO on J30
#define GPIO_REG_SIZE  0x28  // Memory Region GPIO Bank 0 and 1
#define GPIO_NAME  "GPIO"
#define GPIO_DDR_OFFSET  0
#define GPIO_DATA_OUT_OFFSET  1
#define GPIO_DATA_IN_OFFSET  4

// GPIO  definitions
#define GPIO0_0  0
#define GPIO0_1  1
#define GPIO0_2  2
#define GPIO0_6  6
#define GPIO0_13  13
#define GPIO0_15  15

// Commands for ioctl
#define IOCTL_TYPE  'g'
#define READ_LEDS  _IOR(IOCTL_TYPE, 0, int)
#define READ_BUTTONS  _IOR(IOCTL_TYPE, 1, int)

#endif /*OMAPL138Exp_IO_H_*/
