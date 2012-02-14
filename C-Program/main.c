/*
 * main.c
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

// Standard Linux headers
#include     <stdio.h>              // Always include this header
#include     <stdlib.h>             // Always include this header
#include     <signal.h>             // Defines signal-handling functions (i.e. trap Ctrl-C)


// Application headers
#include     "debug.h"
// #include     "audio_thread.h"
#include     "gpio.h"

// Global audio thread environment
// audio_thread_env audio_env = {0};

/* Store previous signal handler and call it */
void (*pSigPrev)(int sig);

// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
    DBG( "Ctrl-C pressed, cleaning up and exiting..\n" );
   //  audio_env.quit = 1;

    if( pSigPrev != NULL )
        (*pSigPrev)( sig );
}


//*****************************************************************************
//*  main
//*****************************************************************************
int main( int argc, char *argv[] )
{
    int   status = EXIT_SUCCESS;

    void *audioThreadReturn;


    // Set the signal callback for Ctrl-C
    pSigPrev = signal( SIGINT, signal_handler );
    DBG( "Registered SIGINT signal handler.\n" );

	// button values
	int button = 10;
	int bButton = 2;
	button =  (bButton*32)+button;
	exportgpio(button);

	//SET DIRECTION
	setdirection(button,1);	

    while(1){

	if(gpioRead(button)){
		//Grab the audio from the microphone while the button is pressed
		//audioThreadReturn = audio_thread_fxn( (void *) &audio_env, button);
		//runs a script that will take the raw file saved and encode it to mp3 useing lame and send via email.
 		system("./../BeagleVNSScript");
		

	}	
    }
}

