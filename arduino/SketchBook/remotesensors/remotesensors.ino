
#include <RemoteReceiver.h>
#include <RemoteTransmitter.h>

#include <SensorReceiver.h>
#include <SensorTransmitter.h>

#include <NewRemoteReceiver.h>
#include <NewRemoteTransmitter.h>

#include <InterruptChain.h>

// need to include although in mytimer.h, otherwise will fail compile
#include <TimerOne.h>

#define DEBUG_LED  13
#define PIN433DATA 2

#include "data.h"
#include "receiver433.h"
#include "mytimer.h"

void setup() {
  pinMode(DEBUG_LED, OUTPUT);
  Serial.begin(BAUDRATE);
  Serial.println("Remote sensor v.01");// "http://randysimons.nl/");
  initData();
  dumpData();
  setupTimer(); 
  setupReceiver(PIN433DATA); // D.2 = 433 MHz receiver

}

void loop() {
  while(true){
    if(seconds % 60 == 0){
      dumpData();
    }
  }
}


