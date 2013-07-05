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
#define ARRAY_SIZE 15	// number of measured values
#define R_REF = 98	// reference resistor

static volatile int elapsed_time = 0;

// Measure 12 values to get at least two complete cycles
static volatile int counter = 0;
static int cycles[ARRAY_SIZE] = {0};

/*
 * Interrupt routine, which prints out the PIN value for the moment
 */
void pt100_interrupt() {
  if (counter < ARRAY_SIZE) {
    int duration = millis() - elapsed_time;
    elapsed_time = millis();
    cycles[counter++] = duration;
  
    printf("Time since last call: %ims\n", duration);
  }
}

/*
 * Routine to find the start of the cycle. Therefore the two lowest values of
 * the cycle need to be found.
 *
 * Returns the address of the last SOF-value.
 */
int* find_start_of_cycle() {
  int* smallest = cycles+1;
  int* sof = smallest;
  
  //find the smallest element
  int i;
  for (i=2;i<5;i++) {
      if (cycles[i] < *smallest) smallest = cycles+i;
  }
  
  /* compare smallest-1 element with smallest+1 element and take the smaller one,
   * which is then the first or second part of the SOF signal. If both are the
   * same we have a problem.
  */
  if (*(smallest-1) < *(smallest+1)) {
    sof = smallest-1; 
  } 
  else if (*(smallest-1) > *(smallest+1)) {
    sof = smallest;
  }
    
  return sof;
}

/*
 * Calculates the resistance of the PT100 sensor in a given cycle.
 * 
 */
int resistance(int* sof) {
  double Noff = *sof + *(sof+1);
  double Nab = *(sof+2);
  double Ncd = *(sof+3);
  
  double resistance = (Ncd - Noff)/(Nab - Noff);
  
  return resistance;
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
    printf ("Waiting ... \n") ; fflush (stdout) ;
  }


  for (;;)
  {
      if (counter < ARRAY_SIZE)
      {
	      delay (100) ;
      }
      else {
	//remove interrupt routine
	wiringPiISR (PT100_PIN, INT_EDGE_RISING, 0);
	break;
      }      
  }
  
  int* sof = find_start_of_cycle();
  int Rpt100 = resistance(sof);
  
  printf("Resistance is futile: %iOhm", Rpt100);

  return 0 ;
}