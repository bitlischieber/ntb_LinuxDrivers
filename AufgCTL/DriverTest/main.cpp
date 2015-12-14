#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "../omapl138exp_io.h"

void waitNegPulse(int);

int main(int argc, char **argv) {

    int dev = open("/dev/omapgpio", O_RDWR);

    if (dev != -1) {
      // successfully opened
      std::cout << "Device opened." << std::endl;

      uint8_t val;
      struct _stopWatchStruct sw;
      
      std::cout << "All LEDs on." << std::endl;
      sw.func = TFUNC_ALL_LEDS_ON;
      ioctl(dev, INIT_TIMER, &sw);
      ioctl(dev, ALL_LEDS_ON, 0);
      ioctl(dev, INIT_TIMER, &sw);
      std::cout << "  Action took " << sw.duration_us << " usec." << std::endl;
      std::cout << "  Press SW0 to continue..." << std::endl;
      waitNegPulse(dev);
      
      std::cout << "All LEDs off." << std::endl;
      sw.func = TFUNC_ALL_LEDS_OFF;
      ioctl(dev, INIT_TIMER, &sw);
      ioctl(dev, ALL_LEDS_OFF, 0);
      ioctl(dev, INIT_TIMER, &sw);
      std::cout << "  Action took " << sw.duration_us << " usec." << std::endl;
      std::cout << "  Press SW0 to continue..." << std::endl;
      waitNegPulse(dev);
      
      std::cout << "LED Pattern 1." << std::endl;
      sw.func = TFUNC_APPLY_PATTERN;
      ioctl(dev, INIT_TIMER, &sw);
      val = 0x05;
      ioctl(dev, APPLY_PATTERN, &val);
      ioctl(dev, INIT_TIMER, &sw);
      std::cout << "  Action took " << sw.duration_us << " usec." << std::endl;
      std::cout << "  Press SW0 to continue..." << std::endl;
      waitNegPulse(dev);
      
      std::cout << "LED Pattern 2." << std::endl;
      sw.func = TFUNC_APPLY_PATTERN;
      ioctl(dev, INIT_TIMER, &sw);
      val = 0x0A;
      ioctl(dev, APPLY_PATTERN, &val);
      ioctl(dev, INIT_TIMER, &sw);
      std::cout << "  Action took " << sw.duration_us << " usec." << std::endl;
      std::cout << "  Press SW0 to continue..." << std::endl;
      waitNegPulse(dev);
      
      
      std::cout << "LED Write." << std::endl;
      sw.func = TFUNC_WRITE;
      ioctl(dev, INIT_TIMER, &sw);
      val = 0x03;
      write(dev, &val, 1);
      ioctl(dev, INIT_TIMER, &sw);
      std::cout << "  Action took " << sw.duration_us << " usec." << std::endl;
      std::cout << "  Press SW0 to exit..." << std::endl;
      waitNegPulse(dev);

      close(dev);
      std::cout << "Driver closed." << std::endl;

    } else {
      // Open failed
      std::cout << "Error, couldn't open device! " << dev << std::endl;
    } // if dev != -1

    return 0;
} // main

void waitNegPulse(int dev) {
  uint8_t btn, btnPrev;
  btn = btnPrev;
  while((btn == btnPrev) || (btn != 0)) {
    btnPrev = btn;
    ioctl(dev, READ_BUTTONS, &btn);
    btn = btn & 0x01;
  } // wend
} // waitNegPulse

