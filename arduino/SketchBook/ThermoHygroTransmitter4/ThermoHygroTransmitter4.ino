#include <Sleep_n0m1.h>
Sleep sleep;
unsigned long sleepTime; //how long you want the arduino to sleep

#include <dht11.h>
#define DHT_DATA_PIN  2
static Dht11 sensor(DHT_DATA_PIN);
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
  Serial.println(Dht11::VERSION);
    
  sleepTime=10000; //10 seconds 
}

void loop(){
  //do stuff
    Serial.println("...waked up");

    Serial.print("sendCount="); Serial.println(sendCount);
    if(sendCount >= sendInterval){
      sendCount=0;
      switch (sensor.read()) {
        case Dht11::OK:
            Serial.print("Humidity (%): ");
            lastHumi=sensor.getHumidity();
            Serial.println(lastHumi);
    
            Serial.print("Temperature (C): ");
            lastTemp=sensor.getTemperature();
            Serial.println(lastTemp);
            bValidData=true;
            break;
    
        case Dht11::ERROR_CHECKSUM:
            Serial.println("Checksum error");
            break;
    
        case Dht11::ERROR_TIMEOUT:
            Serial.println("Timeout error");
            break;
    
        default:
            Serial.println("Unknown error");
            break;
     }//switch
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

