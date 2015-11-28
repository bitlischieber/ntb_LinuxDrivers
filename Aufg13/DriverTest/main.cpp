#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {

    int dev = open("/dev/omapgpio", O_RDWR);

    if (dev != -1) {
      // successfully opened
      std::cout << "Device opened." << std::endl;
      std::cout << "SW0 to decrement, SW1 to increment, booth exit." << std::endl;
      
      char alife = 1;
      char number = 0;
      char swlaststate = 0;
      char swstate = 0;
      
      while (alife) {
	write(dev, &number, 1);
	read(dev, &swstate, 1);
	if (swlaststate != swstate ) {
	  switch (swstate) {
	    case 1: 
	      number = (number == 0) ? 0x0f : number - 1; 
	      break;
	    case 2: 
	      number++; 
	      if (number > 0x0f) number = 0;
	      break;
	    case 3: alife = 0;
	    default: break;
	  } // switch
	  swlaststate = swstate;
	} // if swstate
      } // while      
      close(dev);
      std::cout << "Driver closed." << std::endl;
    } else {
      // Open failed
      std::cout << "Error, couldn't open device! " << dev << std::endl;
    }

    return 0;
}
