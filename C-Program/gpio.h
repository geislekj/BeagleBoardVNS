#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

FILE *fp;

//*****************************************************************************
//*  A Function to export a GPIO Port
//***************************************************************************
void exportgpio(int port);

//*****************************************************************************
//*  A Function to Set the Direction of a GPIO Port
//*  ReadIn: In = 1 or OUT = 0
//***************************************************************************
void setdirection(int port, int readIn);

//*****************************************************************************
//*  Function to Read in a gpio Port
//*****************************************************************************
int gpioRead(int port);

