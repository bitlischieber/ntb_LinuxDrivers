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
#define READ_LEDS _IOR(IOCTL_TYPE, 0, int)
#define READ_BUTTONS _IOR(IOCTL_TYPE, 1, int)
#define WRITE_LEDS _IOR(IOCTL_TYPE, 2, int)
#define ALL_LEDS_ON _IOR(IOCTL_TYPE, 3, int)
#define ALL_LEDS_OFF _IOR(IOCTL_TYPE, 4, int)
#define APPLY_PATTERN _IOR(IOCTL_TYPE, 5, int)
#define GET_NOF_RES _IOR(IOCTL_TYPE, 6, int)
#define INIT_TIMER _IOR(IOCTL_TYPE, 7, int)

//     ioctl struct
typedef struct _ledStruct {
  uint8_t LED_No;    // 0 to 3
  uint8_t LED_State; // 0 = off, 1 = on
} _ledStruct;

// Timer function
enum TIMER_FUNCTION {
  TFUNC_WRITE,
  TFUNC_READ,
  TFUNC_READ_LEDS,
  TFUNC_READ_BUTTONS,
  TFUNC_WRITE_LEDS,
  TFUNC_ALL_LEDS_ON,
  TFUNC_ALL_LEDS_OFF,
  TFUNC_APPLY_PATTERN,
  TFUNC_GET_NOF_RES,
  TFUNC_INIT_TIMER
} ;

typedef struct _stopWatchStruct {
  enum TIMER_FUNCTION func;
  unsigned int duration_us;
} _stopWatchStruct;

#endif /*OMAPL138Exp_IO_H_*/
