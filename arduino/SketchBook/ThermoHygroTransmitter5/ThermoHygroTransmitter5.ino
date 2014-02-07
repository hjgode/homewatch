#include <Sleep_n0m1.h>
Sleep sleep;
unsigned long sleepTime; //how long you want the arduino to sleep

#include <DHT.h>
#define DHT_DATA_PIN  2
static DHT sensor;
static boolean bValidData=false;
    
static byte sendCount=1;
const byte sendInterval=6; //send every x wakeups

int lastTemp=0;
int lastHumi=0;

#define SERIAL_BAUD 9600

#define led 4

 #include <SensorTransmitter.h>
 
 #define TX_DATA_PIN  3
 #define TX_RAND_ID  4
 #define TX_CHANNEL  4
 
 // Initializes a ThermoHygroTransmitter on pin 11, with "random" ID 0, on channel 1.
 ThermoHygroTransmitter transmitter(TX_DATA_PIN, TX_RAND_ID, TX_CHANNEL);
 
 void blinkLED(){
   digitalWrite(led, HIGH);
   delay(500);
   digitalWrite(led, LOW);
 }
 
 void blinkLEDfast(){
   digitalWrite(led, HIGH);
   delay(200);
   digitalWrite(led, LOW);
   delay(200);
   digitalWrite(led, HIGH);
   delay(200);
   digitalWrite(led, LOW);
   delay(200);
   digitalWrite(led, HIGH);
   delay(200);
   digitalWrite(led, LOW);
   delay(200);
 }


void setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("ThermoHygroTransmitter version 4 (power save 2)");
  sensor.setup(DHT_DATA_PIN);
    
  sleepTime=10000; //10 seconds 
}

void loop(){
  //do stuff
    Serial.println("...waked up");
    float humidity = sensor.getHumidity();
    float temperature = sensor.getTemperature();
/*
  Serial.println(humidity, 1);
  Serial.println(temperature, 1);
  Serial.println(sensor.getStatusString());
*/
    if(strcmp(sensor.getStatusString(),"OK")==0){
      bValidData=true;
      lastTemp=(int)temperature;
      lastHumi=(int)humidity;
    }
    else{
      bValidData=false;
    }
    Serial.print("sendCount="); Serial.println(sendCount);
    if(sendCount >= sendInterval){
      sendCount=0;
     // Displays temperatures from -10 degrees Celsius to +20,
     // and humidity from 10% REL to 40% REL, with increments of 2
     if(bValidData) {
       // Temperatures are passed at 10 times the real value,
       // to avoid using floating point math.
       ///transmitter.sendTempHumi(i * 10, i + 20);
       transmitter.sendTempHumi(lastTemp * 10, lastHumi);
       Serial.println("Transmit done");
       blinkLEDfast();
       // Wait two seconds before sending next.
       bValidData=false;
     }//validData
   }//send_loop
   //delay(1000);
  sendCount++;
  blinkLED();

  Serial.println("About to sleep...");
  delay(200);
  sleep.pwrDownMode(); //set sleep mode
  sleep.sleepDelay(sleepTime); //sleep for: sleepTime
}

