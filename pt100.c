/*
 * pt100.c:
 * 	Use interrupts to read falling and rising edges from a gpio pin which is
 * 	connected to a Smartec universal transduce interface, which is again
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

/*
 * Interrupt routine, which prints out the PIN value for the moment
 */
void pt100_interrupt() {
  int value = digitalRead(PT100_PIN);
  
  printf("Current value of pin %i: %i", PT100_PIN, value);
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

  if (wiringPiISR (PT100_PIN, INT_BOTH, &pt100_interrupt) < 0)
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
      delay (100) ;
  }

  return 0 ;
}