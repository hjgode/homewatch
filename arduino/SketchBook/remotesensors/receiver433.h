#ifndef _RECEIVER433_H_
#define _RECEIVER433_H_

#include "data.h"

//forward declarations
void showOldCode(unsigned long receivedCode, unsigned int period);
void showNewCode(NewRemoteCode receivedCode);
void showTempHumi(byte *data);
void setupReceiver();


// shows the received code sent from an old-style remote switch
void showOldCode(unsigned long receivedCode, unsigned int period) {
  // Print the received code.
  Serial.print("Code: ");
  Serial.print(receivedCode);
  Serial.print(", period: ");
  Serial.print(period);
  Serial.println("us.");
}

// Shows the received code sent from an new-style remote switch
void showNewCode(NewRemoteCode receivedCode) {
  // Print the received code.
  Serial.print("Addr ");
  Serial.print(receivedCode.address);
  
  if (receivedCode.groupBit) {
    Serial.print(" group");
  } else {
    Serial.print(" unit ");
    Serial.print(receivedCode.unit);
  }
  
  switch (receivedCode.switchType) {
    case 0:
      Serial.print(" off");
      break;
    case 1:
      Serial.print(" on");
      break;
    case 2:
      Serial.print(" dim level");
      Serial.print(receivedCode.dimLevel);
      break;
  }
  
  Serial.print(", period: ");
  Serial.print(receivedCode.period);
  Serial.println("us.");
}

//updateSensorData array
void updateSensorData(byte channel, int temp, short int humi){
  static uint32_t last_time=0;
  if(1<=channel && channel<=MAX_CHANNEL)
  {
    int idxChannel = channel - 1;
    if(bGotValidTime){
      strcpy(sensorData[idxChannel].strDay, strDay);
      strcpy(sensorData[idxChannel].strClock, strClock);
      sensorData[idxChannel].time_long = time_long;
      last_time=time_long;
    }
    else{
      sprintf(sensorData[idxChannel].strDay, "-");
      sprintf(sensorData[idxChannel].strClock,"-");
    }
    //no time, just use millis
    sensorData[idxChannel].time_long=millis();
    sensorData[idxChannel].channel=channel;
    sensorData[idxChannel].temp=temp;    
    sensorData[idxChannel].humidity=humi;
  }
}

// Shows the received sensor data
void showTempHumi(byte *data) {
  // is data a ThermoHygro-device?
  if ((data[3] & 0x1f) == 0x1e) {
    // Yes!
    byte channel, randomId;
    int temp;
    short int humidity;

    // Decode the data
    SensorReceiver::decodeThermoHygro(data, channel, randomId, temp, humidity);
  
  //store data
  updateSensorData(channel, temp, humidity);
  
    // Print temperature. Note: temp is 10x the actual temperature!
    Serial.print("Temperature: ");
    if(abs(temp)!=temp){
      Serial.print("-");
      temp=abs(temp);
    }
    Serial.print(temp / 10); // units
    Serial.print('.');
    Serial.print(temp % 10); // decimal
    
        // Print humidity
    Serial.print(" deg, Humidity: ");
    Serial.print(humidity);
    Serial.print("% REL");

    // Print channel
    Serial.print(", Channel: ");
    Serial.println(channel, DEC); 
  }
}

void setupReceiver(int PIN_433){
  // Interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  RemoteReceiver::init(-1, PIN_433, showOldCode);
  
  // Again, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  NewRemoteReceiver::init(-1, PIN_433, showNewCode);

  // And once more, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  SensorReceiver::init(-1, showTempHumi);

  // On interrupt, call the interrupt handlers of remote and sensor receivers
  InterruptChain::addInterruptCallback(0, RemoteReceiver::interruptHandler);
  InterruptChain::addInterruptCallback(0, NewRemoteReceiver::interruptHandler);
  InterruptChain::addInterruptCallback(0, SensorReceiver::interruptHandler);
}

#endif //_RECEIVER433_H_
