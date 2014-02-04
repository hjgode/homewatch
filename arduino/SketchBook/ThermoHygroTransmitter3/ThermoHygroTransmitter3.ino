#include <arduino.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#include <dht11.h>
#define DHT_DATA_PIN  2

static byte sendCount=0;
const byte sendInterval=8; //send every 8 wakeups

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

#ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile boolean f_wdt=1;

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  //   INSERTED from setup()
  // CPU Sleep Modes 
  // SM2 SM1 SM0 Sleep Mode
  // 0    0  0 Idle
  // 0    0  1 ADC Noise Reduction
  // 0    1  0 Power-down
  // 0    1  1 Power-save
  // 1    0  0 Reserved
  // 1    0  1 Reserved
  // 1    1  0 Standby(1)

  cbi( SMCR,SE );      // sleep enable, power down mode
  cbi( SMCR,SM0 );     // power down mode
  sbi( SMCR,SM1 );     // power down mode
  cbi( SMCR,SM2 );     // power down mode
  //  END INSERTED from setup()
  
  if (ii > 9 ) ii=9;
    bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
    bb|= (1<<WDCE);
  ww=bb;
  Serial.println(ww);

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);


}
//****************************************************************  
// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {

  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

    sleep_disable();                     // System continues execution here when watchdog timed out 
    sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON

}

void setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("ThermoHygroTransmitter version 3 (power save 1)");
  Serial.println(Dht11::VERSION);
    
  setup_watchdog(9); 
}

void loop(){
  //do stuff
  if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
      Serial.println("...waked up");
      f_wdt=0;       // reset flag
      static Dht11 sensor(DHT_DATA_PIN);
      static boolean bValidData=false;
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
  }  
  Serial.println("About to sleep...");
  delay(2000);
  system_sleep();

//    pinMode(pinLed,OUTPUT); // set all ports into state before sleep
//    pinMode(pinPiezoSpeaker,OUTPUT); // set all ports into state before sleep
}

