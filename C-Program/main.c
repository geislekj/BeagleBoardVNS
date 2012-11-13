/*
 * main.c
 *
 */

// Standard Linux headers
#include     <stdio.h>              // Always include this header
#include     <stdlib.h>             // Always include this header
#include     <signal.h>             // Defines signal-handling functions (i.e. trap Ctrl-C)

// Application headers
#include     "debug.h"
#include     "audio_thread.h"
#include     "gpio.h"

#define AUDIOTHREADATTRSCREATED 0x4
#define AUDIOTHREADCREATED      0x8

// Global audio thread environment
 audio_thread_env audio_env = {0};

/* Store previous signal handler and call it */
void (*pSigPrev)(int sig);

// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
    DBG( "Ctrl-C pressed, cleaning up and exiting..\n" );
     audio_env.quit = 1;

    if( pSigPrev != NULL )
        (*pSigPrev)( sig );
}


//*****************************************************************************
//*  main
//*****************************************************************************
int main( int argc, char *argv[] )
{
	// button values
	int button = 16;
	int bButton = 1;
	int bValue = 0;
	int recording = 0;
	button =  (bButton*32)+button;
	audio_env.button = button;
	exportgpio(button);
	
	//create thread attributes
    	pthread_t audioThread;

	//SET DIRECTION
	setdirection(button,1);	
	
	//	printf("ENTER");

	while(1){
		//Gets the button value
		//printf("Button:%d",bValue);
		bValue = gpioRead(button);

		//Starts the thread to record audio
		if(bValue && !recording){
			printf("Button Pushed");
			//Grab the audio from the microphone while the button is pressed
			audio_thread_fxn( (void *) &audio_env);

			/*			
			if(launch_pthread(&audioThread,REALTIME,99, audio_thread_fxn, (void *) &audio_env) != thread_SUCCESS){
			   	ERR("pthread create failed for audio thread\n");
				//status = EXIT_FAILURE;
				//goto cleanup;
			}
			*/
			recording = 1;
	

		}	
	
		// Sends the audio on button release
		if(!bValue && recording){
			printf("Button Released");

			//runs a script that will take the raw file saved and encode it to mp3 useing lame and send via email.
	 		system("./../BeagleVNSScript");
			
			//reset recording flag
			recording = 0;

			//exit program
			break;
		}
	 }
}

