#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    std::cout << "Driver Test" << std::endl;
    
    int dev = open("/dev/SampleDriver", O_RDONLY);
    
    if (dev != -1) {
      // successfully opened
      std::cout << "Device opened." << std::endl;
      close(dev);
      std::cout << "Driver closed." << std::endl;
    } else{
      // Open failed
      std::cout << "Error, couldn't open device! " << dev << std::endl;
    }    
    
    return 0;
}
