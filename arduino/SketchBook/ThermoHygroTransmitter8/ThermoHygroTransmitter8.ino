//added LCD
//adding intelligence...
//added newBuffer

#include <Sleep_n0m1.h>
Sleep sleep;
unsigned long sleepTime; //how long you want the arduino to sleep

#include <DHT.h>
#define DHT_DATA_PIN  2
static DHT sensor;
static boolean bValidData=false;

#define lcd_rs  8
#define lcd_enable  9
#define lcd_backlight 10
// include the library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(lcd_rs, lcd_enable, 4, 5, 6, 7);

#include "newBuffer.h"
newBuffer send_buffer;
unsigned char state;

static byte sendCount=1;
const byte sendInterval=6; //send every x wakeups

int lastTemp=0;
int lastHumi=0;

#define SERIAL_BAUD 9600

#define led 13

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

void showLCD(int temp, int humi, unsigned char state){
  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.print("*C ");
  lcd.print(humi);
  lcd.print("%");
  lcd.print("  ");
  if(state==0)
    lcd.print("off");
  else
    lcd.print("ON");
  lcdON();
//  delay(3000);
//  lcdOFF();
}

void lcdON(){
  digitalWrite(lcd_backlight, HIGH);
}
void lcdOFF(){
  digitalWrite(lcd_backlight, LOW);
}

void setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("ThermoHygroTransmitter version 4 (power save 2)");
  sensor.setup(DHT_DATA_PIN);
    
  sleepTime=10000; //10 seconds 
  
  pinMode(lcd_backlight, OUTPUT);

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Temp. / Feuchte");
//  lcdOFF();  
  lcdON();
  // Initialize the send buffer that we will use to send data
  //ByteBuffer send_buffer;
  newBuffer send_buffer;
  send_buffer.init(5);
  send_buffer.setTreshold(5);
  send_buffer.setMaxDuration1(10);}

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
      showLCD(lastTemp, lastHumi, send_buffer.getState1());
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

        send_buffer.push(lastHumi);
        state=send_buffer.getState1();
        int x=0;
        Serial.println("============================");
        for(x=0; x<send_buffer.getSize1(); x++){
          Serial.print("["); 
          Serial.print(send_buffer.peek1(x));
          Serial.print("]"); 
        }
        Serial.println("\n============================");

       // Temperatures are passed at 10 times the real value,
       // to avoid using floating point math.
       ///transmitter.sendTempHumi(i * 10, i + 20);
       //HACK: send temp with ending in 5 (which is .5 degree clesius if state==1
       // if state==0 send temp with ending in 0
       if(state==1)
         transmitter.sendTempHumi(lastTemp * 10 + 5, lastHumi);
       else
         transmitter.sendTempHumi(lastTemp * 10, lastHumi);
       
       Serial.println("Transmit done");
       blinkLEDfast();
       // Wait two seconds before sending next.
       bValidData=false;
       showLCD(lastTemp, lastHumi, state);
       
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

