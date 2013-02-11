//mytimer.h

#ifndef _MYTIMER_H_
#define _MYTIMER_H_

#include <TimerOne.h>

volatile unsigned int seconds=0;

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------

void timerIsr()
{
  noInterrupts();
  seconds++;
  if(seconds % 2 ==0)
      digitalWrite(DEBUG_LED, HIGH);   // set the LED on
  else
    digitalWrite(DEBUG_LED, LOW);   // set the LED on
  //Serial.println(".");
  /*
  if(seconds % 60){
    dumpData();
  }
  */
  if(seconds % 600 == 0)
    ;//pushUpdate();
  if(seconds<3600)
     return; //only continue every hour
    // Toggle LED
    //digitalWrite( 13, digitalRead( 13 ) ^ 1 );
    ;//getNTPtime();
  seconds=0;
  interrupts();
}  

///---------------------------
/// Setup Timer
///---------------------------
void setupTimer(){
  Timer1.initialize(1000000); // set a timer of length 100.000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}
#endif //ifndef _MYTIMER_H_
