#include "gpio.h"

//*****************************************************************************
//*  A Function to export a GPIO Port
//***************************************************************************
void exportgpio(int port){
	
	//Using sysfs we need to write the 3 digit gpio number to /sys/class/gpio/export
	//This will create the folder /sys/class/gpio/gpio37
	if ((fp = fopen(SYSFS_GPIO_DIR "/export", "ab")) == NULL)

		{
			printf("Cannot open export file.\n");
			exit(1);
		}
	//Set pointer to begining of the file
		rewind(fp);
		//Write our value of "37" to the file
		char set_value[5];
		sprintf(&set_value, "%d\0",port);
		fwrite(&set_value, sizeof(char), 3, fp);
		fclose(fp);
	
	printf("...export file accessed, new pin now accessible\n");
	
}

//*****************************************************************************
//*  A Function to Set the Direction of a GPIO Port
//*  ReadIn: In = 1 or OUT = 0
//***************************************************************************
void setdirection(int port, int readIn){
	char filestringval[40];
	sprintf(&filestringval, "%s%s%d%s\0", SYSFS_GPIO_DIR, "/gpio", port, "/direction");
	if ((fp = fopen(filestringval, "rb+")) == NULL)
	{
		printf("Cannot open direction file.\n");
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "out" to the file
	char set_value[5];
	if(readIn == 1){
		strcpy(set_value,"in");
	}else{
		strcpy(set_value,"out");
	}
	fwrite(&set_value, sizeof(char), 3, fp);
	fclose(fp);
	printf("...direction set to output\n");		
}

//*****************************************************************************
//*  Function to Read in a gpio Port
//*****************************************************************************
int gpioRead(int port){
	char* filestringval[40];
	sprintf(&filestringval, "%s%s%d%s\0", SYSFS_GPIO_DIR, "/gpio", port, "/value");
	if ((fp = fopen(filestringval, "rb+")) == NULL)
	{
		printf("Cannot open value file.\n");
		exit(1);
	}	

	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "0" to the file 
	char inString[10];
	fgets(&inString, 10, fp);
	fflush(fp);
	fclose(fp);
//	printf("...value set to 0...\n");	
	int i = atoi(&inString);
//	printf("i = %d\n", i);
	return i;
}

//*****************************************************************************
//*  Function to Write in a gpio Port
//*****************************************************************************
void gpioWrite(int port, int trigger){
	char* filestringval[40];
	sprintf(&filestringval, "%s%s%d%s\0", SYSFS_GPIO_DIR, "/gpio", port, "/value");
	if ((fp = fopen(filestringval, "rb+")) == NULL)
	{
		printf("Cannot open value file.\n");
		exit(1);
	}
	
	if(trigger == 1){
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "1" to the file 
			char set_value[5];
			strcpy(set_value,"1");
			fwrite(&set_value, sizeof(char), 1, fp);
			fflush(fp);
//			printf("...value set to 1...\n");
	}else{
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "0" to the file 
			char set_value[5];
			strcpy(set_value,"0");
			fwrite(&set_value, sizeof(char), 1, fp);
			fflush(fp);
//			printf("...value set to 0...\n");	
	}	
	fclose(fp);
}

