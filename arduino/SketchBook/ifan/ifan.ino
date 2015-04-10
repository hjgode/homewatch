//ifan
#include <Arduino.h>

#include "iFanControl.h"

#define STAT_USE_STDEV
#include "statistic.h"
Statistic myStats;

void setup()
{
        // initialize the digital pin as an output.
        // Pin 13 has an LED connected on most Arduino boards:
        pinMode(13, OUTPUT);
        iFanControl iFan;
        byte vals[] = {63,63,62,63,64,63,64,63,63,62,63,64,63,64,0};

    iFan.addVals(vals);

    myStats.clear();
    int i=0;
    do{
      myStats.add(vals[i]);
      i++;
    }while (vals[i]!=0);
    float mmin=myStats.minimum();
    float mmax=myStats.maximum();
    float dev =myStats.pop_stdev();
    float avg = myStats.average();

}

void loop()
{
        digitalWrite(13, HIGH);   // set the LED on
        delay(1000);              // wait for a second
        digitalWrite(13, LOW);    // set the LED off
        delay(1000);              // wait for a second
}

