#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "../omapl138exp_io.h"

int main(int argc, char **argv) {

    int dev = open("/dev/omapgpio", O_RDWR);

    if (dev != -1) {
      // successfully opened
      std::cout << "Device opened." << std::endl;
      std::cout << "Push SW0 and SW1 to exit...or wait 30 seconds." << std::endl;

      char alife = 1;
      char number = 1;
      char swstate = 0;
      time_t stim, ltim, tim;

      stim = time(NULL);
      ltim = stim;
      
      while (alife) {
	
	if (ltim < tim) {
	  // update only every second
	  ltim = tim;
	  write(dev, &number, 1);
	  number = number << 1;
	  if (number > 0x08) number = 1;
	} // if
	
	// kernel log: LED state
	ioctl(dev, READ_LEDS, 0);
	// kernel log: Tact switch state
	ioctl(dev, READ_BUTTONS, 0);
	
	// terminate after 30 secs...
	tim = time(NULL);
	if (difftime(tim, stim) > 30) alife = false;
	// ...of if booth buttons are pressed
	read(dev, &swstate, 1);
	if (swstate == 0x03) alife = false;
	
	
      } // while
      
      close(dev);
      std::cout << "Driver closed." << std::endl;
      
    } else {
      // Open failed
      std::cout << "Error, couldn't open device! " << dev << std::endl;
    }

    return 0;
}
