/*
 * pt100.c:
 * 	Use interrupts to read rising edges from a gpio pin which is
 * 	connected to a Smartec universal transducer interface, which is again
 * 	connected to a PT100 platinum resistance sensor. 
 * 
 * 	Prints out the temperature in degree Celsius.
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
#define ARRAY_SIZE 85	// number of measured values
#define CYCLE_LENGTH 5  // length of one cycle
#define R_REF 100	// reference resistor

static volatile int elapsed_time = 0;

// Measure ARRAY_SIZE values to get at least ARRAY_SIZE/CYCLE_LENGTH-2 complete cycles
static volatile int counter = 0;
static int cycles[ARRAY_SIZE] = {0};

/*
 * Interrupt routine, which calcutes the durations between two rising edges
 */
void pt100_interrupt() {
  if (counter < ARRAY_SIZE) {
    int duration = micros() - elapsed_time;
    elapsed_time = micros();
    cycles[counter++] = duration;
  }
}

/*
 * Routine to find the start of the cycle. Therefore the two lowest values of
 * the cycle need to be found.
 *
 * Returns the address of the first SOF-value.
 */
int* find_start_of_cycle() {
  int* smallest = cycles+1;
  int* sof = smallest;
  
  //find the smallest element in a range of 2-times the CYCLE_LENGTH
  int i;
  for (i=2;i<10;i++) {
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
  
  double resistance = (Ncd - Noff)*R_REF/(Nab - Noff);
  
  return resistance;
}

/*
 * Calculate average resistance
 */
float average_resistance(int *sof) {
  int valid_cycles;
  float Rsum = 0;
  for (valid_cycles=0;valid_cycles<ARRAY_SIZE-(CYCLE_LENGTH*2);valid_cycles+=CYCLE_LENGTH) {
    Rsum += resistance(sof+valid_cycles);
  }
  return (CYCLE_LENGTH*Rsum)/valid_cycles;  
}

/**
 * Map resistance to temperature. Ripped and modified from
 * http://en.wikipedia.org/wiki/Resistance_thermometer#The_function_for_temperature_value_acquisition_.28C.2B.2B.29
 */

float GetPt100Temperature(float r)
{
    float const Pt100[] = { 80.31,   82.29,  84.27,  86.25,  88.22,  90.19,  92.16,  94.12,  96.09,  98.04,
                            100.0,  101.95, 103.9,  105.85, 107.79, 109.73, 111.67, 113.61, 115.54, 117.47,
                            119.4,  121.32, 123.24, 125.16, 127.07, 128.98, 130.89, 132.8,  134.7,  136.6,
                            138.5,  140.39, 142.29, 157.31, 175.84, 195.84 };
    int t = -50, i = 0, dt = 0;
    if (r > Pt100[0])
      while (250 > t) {
        dt = (t < 110) ? 5 : (t > 110) ? 50 : 40;
        if (r < Pt100[++i])
          return t + (r - Pt100[i-1]) * dt / (Pt100[i] - Pt100[i-1]);
        t += dt;
      };
     return t;
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
    fflush (stdout) ;
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
  double Rpt100 = average_resistance(sof);
  
  printf("%f", GetPt100Temperature(Rpt100));

  return 0 ;
}