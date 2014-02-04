#include <dht11.h>
#define DHT_DATA_PIN  2

long postinterval = 25 * 1000; //25 seconds

int lastTemp=0;
int lastHumi=0;

#define SERIAL_BAUD 9600

#define led 4

/*
 * This sketch sends (bogus) thermo / hygro data to a remote weather sensors made by Cresta.
 * 
 * Setup:
 *  - connect transmitter input of a 433MHz transmitter to digital pin 11
 *  - On the weather station, activate the "scan" function for channel 1.
 */
 
 #include <SensorTransmitter.h>
 
 #define TX_DATA_PIN  3
 #define TX_RAND_ID  4
 #define TX_CHANNEL  4
 
 // Initializes a ThermoHygroTransmitter on pin 11, with "random" ID 0, on channel 1.
 ThermoHygroTransmitter transmitter(TX_DATA_PIN, TX_RAND_ID, TX_CHANNEL);
 
 static uint32_t send_timer;
 
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
 
 void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.print("Dht11 Lib version ");
    Serial.println(Dht11::VERSION);
 }
 
 void loop() {
    static Dht11 sensor(DHT_DATA_PIN);
    static boolean bValidData=false;
    if(millis() > send_timer){
      send_timer = millis() + postinterval; 
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
   }//send_timer
  delay(1000);
  blinkLED();
 }//loop
 
