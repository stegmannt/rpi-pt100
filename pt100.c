/*
 * pt100.c:
 * 	Use interrupts to read rising edges from a gpio pin which is
 * 	connected to a Smartec universal transducer interface, which is again
 * 	connected to a PT100 platinum resistance sensor. 
 *	
 *	http://www.smartec.nl/pdf/DSUTI.pdf 
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#define PT100_PIN 4	// PT100 is connected to GPIO 23

static volatile int elapsed_time = 0;

// Measure 12 values to get at least 2 complete cycles
static volatile int counter = 0;
static int cycles[12] = {0};

/*
 * Interrupt routine, which prints out the PIN value for the moment
 */
void pt100_interrupt() {
  int duration = millis() - elapsed_time;
  elapsed_time = millis();
  cycles[counter++] = duration;
  
  printf("Time since last call: %ims\n", duration);
}

/*
 * main function ripped and modified from WiringPi/examples/isr.c
 */
int main(void) {
  
  if (wiringPiSetup () < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiISR (PT100_PIN, INT_EDGE_RISING, &pt100_interrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }
  else
  { 
    printf ("Waiting ... ") ; fflush (stdout) ;
  }


  for (;;)
  {
      if (counter < 12)
      {
	      delay (100) ;
      }
      else {
	break;
      }      
  }

  return 0 ;
}