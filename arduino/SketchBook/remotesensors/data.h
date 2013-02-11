#ifndef _DATA_H_
#define _DATA_H_

// using redirect stdout to serial
// see http://playground.arduino.cc/Main/Printf
// we need fundamental FILE definitions and printf declarations
#include <stdio.h>
#include <avr/pgmspace.h>

// create a FILE structure to reference our UART output function

static FILE uartout = {0} ;
// create a output function
// This works because Serial.write, although of
// type virtual, already exists.
static int uart_putchar (char c, FILE *stream)
{
    Serial.write(c) ;
    return 0 ;
}

#include "myconstants.h"

  char strDay[22];        //string to hold day
  char strClock[22];      //string to hold clock
  uint32_t time_long;
  boolean bGotValidTime;
  
    
//store last values
typedef struct {
    char strDay[22];
    char strClock[22];
    uint32_t time_long;
    byte channel;
    int temp;
    short int humidity;
}SENSORDATA;

#define MAX_CHANNEL 3
SENSORDATA sensorData[MAX_CHANNEL];

void initData(){
     // fill in the UART file descriptor with pointer to writer.
   fdev_setup_stream (&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);

   // The uart is the standard output device STDOUT.
   stdout = &uartout ;
   
  for(int i=0; i<MAX_CHANNEL; i++){
    sprintf(sensorData[i].strDay, "%s", "2013-01-20");
    sprintf(sensorData[i].strClock, "%s", "12:00");
    sensorData[i].time_long=0;
    sensorData[i].channel=i+1;
    sensorData[i].temp=0;
    sensorData[i].humidity=0;
  }
}
void dumpData(){

  for(int i=1; i<=MAX_CHANNEL; i++){
    printf_P( webString, sensorData[i-1].channel, 
      sensorData[i-1].temp, 
      sensorData[i-1].humidity, 
      sensorData[i-1].time_long);
//    Serial.printf_P(webString, sensorData[i-1].channel, sensorData[i-1].temp, sensorData[i-1].humidity);
    printf("\r\n");
  }
}

#endif  //_DATA_H_
