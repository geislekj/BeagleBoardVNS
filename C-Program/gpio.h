//*****************************************************************************
//*  A Function to export a GPIO Port
//***************************************************************************
void exportgpio(int port);

//*****************************************************************************
//*  A Function to Set the Direction of a GPIO Port
//*  ReadIn: In = 1 or OUT = 0
//***************************************************************************
void setdirectionout(int port, int readIn);

//*****************************************************************************
//*  Function to Read in a gpio Port
//*****************************************************************************
int gioRead(int port);
