/*
 * main.c
 *
 */

// Standard Linux headers
#include     <stdio.h>              // Always include this header
#include     <stdlib.h>             // Always include this header
#include     <signal.h>             // Defines signal-handling functions (i.e. trap Ctrl-C)
#include     <fcntl.h>                          // Defines open, read, write methods
#include     <unistd.h>                         // Defines close and sleep methods
//#include     <string.h>                         // Defines memcpy
#include     <alsa/asoundlib.h>			// ALSA includes


// Application headers
#include     "debug.h"
//#include     "audio_thread.h"
#include     "gpio.h"
#include     "audio_input_output.h"             // Audio driver input and output functions

//* ALSA and Mixer devices **
#define     SOUND_DEVICE     "plughw:0,0"	// This uses line in
//#define     SOUND_DEVICE     "plughw:1,0"	// This uses the PS EYE mikes

//* Output file name **
#define     OUTFILE          "/tmp/audio.raw"

//* The sample rate of the audio codec **
#define     SAMPLE_RATE      8000

//* The gain (0-100) of the left channel **
#define     LEFT_GAIN        100

//* The gain (0-100) of the right channel **
#define     RIGHT_GAIN       100

//*  Parameters for audio thread execution **
#define     BLOCKSIZE        48000

// Success and failure definitions for the thread
#define     AUDIO_THREAD_SUCCESS     ( ( void * ) 0 )
#define     AUDIO_THREAD_FAILURE     ( ( void * ) - 1 )

// The levels of initialization for initMask
#define     INPUT_ALSA_INITIALIZED      0x1
#define     INPUT_BUFFER_ALLOCATED      0x2
#define     OUTPUT_FILE_OPENED          0x4

// Thread environment definition (i.e. what it needs to operate)
typedef  struct  audio_thread_env
{
    int quit;                // Thread will run as long as quit = 0
    int button;
} audio_thread_env;

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

// Variables and definitions
// *************************

	// button values
	int button = 16;
	int bButton = 1;
	int bValue = 0;
	int recording = 0;
	button =  (bButton*32)+button;
	audio_env.button = button;
	exportgpio(button);

	// led light
	int led = 17;
	int bled = 1;
	led = (bled*32)+led;
	exportgpio(led);

	//SET DIRECTION
	setdirection(button,1);	
	setdirection(led,0);

	//turn off led
	gpioWrite(led,1);

	
	// Thread parameters and return value
        void             * status = AUDIO_THREAD_SUCCESS;      // < see above
        unsigned  int   initMask =  0x0;		// Used to only cleanup items that were init'd

        // Input and output driver variables
        snd_pcm_uframes_t exact_bufsize;
        snd_pcm_t	*pcm_capture_handle;

        FILE          * outfile = NULL;	// Output file pointer (i.e. handle)
        int   blksize = BLOCKSIZE;		// Raw input or output frame size
        char *inputBuffer = NULL;		// Input buffer for driver to read into

// Setup audio input device
// ************************

        // Open an ALSA device channel for audio input
        exact_bufsize = blksize/BYTESPERFRAME;

        if( audio_io_setup( &pcm_capture_handle, SOUND_DEVICE, SAMPLE_RATE, 
	        SND_PCM_STREAM_CAPTURE, &exact_bufsize ) == AUDIO_FAILURE )
        {
                ERR( "Audio_input_setup failed in audio_thread_fxn\n\n" );
                status = AUDIO_THREAD_FAILURE;
                goto cleanup;
        }
        DBG( "exact_bufsize = %d\n", (int) exact_bufsize);

        // Record that input OSS device was opened in initialization bitmask
        initMask |= INPUT_ALSA_INITIALIZED;

        blksize = exact_bufsize*BYTESPERFRAME;
        // Create input buffer to read into from OSS input device
        if( ( inputBuffer = malloc( blksize ) ) == NULL )
        {
           ERR( "Failed to allocate memory for input block (%d)\n", blksize );
           status = AUDIO_THREAD_FAILURE;
           goto  cleanup ;
        }

       DBG( "Allocated input audio buffer of size %d to address %p\n", blksize,inputBuffer );

        // Record that the input buffer was allocated in initialization bitmask
        initMask |= INPUT_BUFFER_ALLOCATED;

// Thread Execute Phase -- perform I/O and processing
// **************************************************
	
        while(1){
		//Gets the button value
		//printf("Button:%d",bValue);
		bValue = gpioRead(button);

		
		// Read capture buffer from ALSA input device
                if( snd_pcm_readi(pcm_capture_handle, inputBuffer, blksize/BYTESPERFRAME) < 0 )
  	        {
               	    snd_pcm_prepare(pcm_capture_handle);
                 }

		//Starts to record audio
		if(bValue && !recording){
                	printf("Button Pushed");
      		       recording = 1;		
                         
                       // Open a file for record
                       outfile = fopen(OUTFILE, "w");

                       if( outfile == NULL )
                       {
                          ERR( "Failed to open file %s\n", OUTFILE );
                          status = AUDIO_THREAD_FAILURE;
                          goto  cleanup ;
                       }

                       DBG( "Opened file %s with FILE pointer = %p\n", OUTFILE, outfile );

                       // Record that input OSS device was opened in initialization bitmask
                       initMask |= OUTPUT_FILE_OPENED;

                       // Processing loop
                       DBG( "Entering audio_thread_fxn processing loop\n" );
		       gpioWrite(led,0);
                }

    		//saves to file while recording.
                if(bValue && recording){
	                if( fwrite( inputBuffer, sizeof( char ), blksize, outfile ) < blksize )
                        {
                             ERR( "Error writing the data to FILE pointer %p\n", outfile );
                             status = AUDIO_THREAD_FAILURE;
                             goto cleanup;
                        }
                }	
	
		// Sends the audio on button release
		if(!bValue && recording){
			printf("Button Released");
			gpioWrite(led,1);

                        DBG( "Closing output file at FILE ptr %p\n", outfile );
                        fclose( outfile );
			
			DBG( "Freeing audio input buffer at location %p\n", inputBuffer );
        		free( inputBuffer );
       			DBG( "Freed audio input buffer at location %p\n", inputBuffer );

			// Create input buffer to read into from OSS input device
			if( ( inputBuffer = malloc( blksize ) ) == NULL )
			{
			   ERR( "Failed to allocate memory for input block (%d)\n", blksize );
			   status = AUDIO_THREAD_FAILURE;
			   goto  cleanup ;
			}

			//runs a script that will take the raw file saved and encode it to mp3 useing lame and send via email.
	 		system("./../BeagleVNSScript");
			
			//reset recording flag
			recording = 0;
		}
	 }

// Thread Delete Phase -- free up resources allocated by this file
// ***************************************************************

cleanup:

    DBG( "Starting audio thread cleanup to return resources to system\n" );

    // Close the audio drivers
    // ***********************
    //  - Uses the initMask to only free resources that were allocated.
    //  - Nothing to be done for mixer device, as it was closed after init.

    // Close input OSS device
    if( initMask & INPUT_ALSA_INITIALIZED )
        if( audio_io_cleanup( pcm_capture_handle ) != AUDIO_SUCCESS )
        {
            ERR( "audio_input_cleanup() failed for file descriptor %d\n", (int) pcm_capture_handle );
            status = AUDIO_THREAD_FAILURE;
        }

    // Free allocated buffers
    // **********************

    // Free input buffer
    if( initMask & INPUT_BUFFER_ALLOCATED )
    {
        DBG( "Freeing audio input buffer at location %p\n", inputBuffer );
        free( inputBuffer );
        DBG( "Freed audio input buffer at location %p\n", inputBuffer );
    }

    // Return from audio_thread_fxn function
    // *************************************

    // Return the status at exit of the thread's execution
    DBG( "Audio thread cleanup complete. Exiting audio_thread_fxn\n" );
}


