//added LCD
//adding intelligence...
//added newBuffer
//added RCswitch

#include <RCSwitch.h>
RCSwitch mySwitch;// = RCSwitch();
#define RC_SWITCH_SYSTEM_CODE "00001"
#define RC_SWITCH_SWITCH_CODE "00001"

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
char* lcdAnim[] = {"|","/","-","\\"};

#include "newBuffer.h"
newBuffer send_buffer;
unsigned char state;

static byte sendCount=1;
const byte sendInterval=6; //send every x wakeups
static byte sendOnOff=1; //counter to control when On/Off will be send, 
//needs to be different than normal Temp/Humi send to have some gap between the two sends

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
  lcd.setCursor(0, 1); //setCursor(col,row)
  lcd.print(temp);
  lcd.print("*C ");
  lcd.print(humi);
  lcd.print("%");
  lcd.print("  ");
  if(state==0)
    lcd.print("off");
  else
    lcd.print("ON ");
    
  //read long time Min value
  byte bMin = send_buffer.getMin2();
  
  lcd.print(" ");
  lcd.print(bMin);
  
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

//switch ELRO wireless power outlet
void switchOnOff(bool bOnOff){
  if(bOnOff){
    Serial.println("SWITCH ON");
    mySwitch.switchOn(RC_SWITCH_SYSTEM_CODE, RC_SWITCH_SWITCH_CODE);
  }
  else{
    Serial.println("SWITCH OFF");
    mySwitch.switchOff(RC_SWITCH_SYSTEM_CODE, RC_SWITCH_SWITCH_CODE);
  }
}

void animateLCD(){
  static int iCnt=0;
  lcd.setCursor(14,0);
  lcd.print(lcdAnim[iCnt]);
  iCnt++;
  if(iCnt==4)
    iCnt=0;
}

void setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("ThermoHygroTransmitter version 10.1 (power save 2)");
  sensor.setup(DHT_DATA_PIN);
    
  sleepTime=10000; //10 seconds 
  
  pinMode(lcd_backlight, OUTPUT);

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Temp/Feuchte"); //12 chars
//  lcdOFF();  
  lcdON();
  // Initialize the send buffer that we will use to send data
  //ByteBuffer send_buffer;
  send_buffer.init(5);
  send_buffer.setTreshold(5);
  send_buffer.setMaxDuration1(10);

  //init RCswitch instance
  mySwitch = RCSwitch();
  // Transmitter is connected to Arduino Pin #3  
  // shared with SensorTransmitter code 
  mySwitch.enableTransmit(TX_DATA_PIN);
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(1);
  
  // Optional set number of transmission repetitions.
  //mySwitch.setRepeatTransmit(2); //stops working!
  
  //initialy switch power outlet OFF
  switchOnOff(false);
}

void loop(){
  //do stuff
    animateLCD();
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
    if(sendCount >= sendInterval){ //update only every 60 seconds
      sendCount=0;
     // Displays temperatures from -10 degrees Celsius to +20,
     // and humidity from 10% REL to 40% REL, with increments of 2
     if(bValidData) {

        send_buffer.push((byte)lastHumi);
        state=send_buffer.getState1();
        
        //on OFF switching moved outside loop
        //to switch OFF if not state! and switched erratically by another sender
        
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
       //showLCD(lastTemp, lastHumi, state); //movded ouside loop
       
     }//validData
     else{
     }//validdData
   }//send_loop

  Serial.print("sendOnOff="); Serial.println(sendOnOff);
   //need to send OnOff async to temp/humi, some Receivers might not see all transmits if to low gap
   if(sendOnOff>=4)
   {
    sendOnOff=0;
    //send switch command, even if data is not valid
    //ensure switch is off/on as state defines!
    if(state==0)
     switchOnOff(false);
    else
     switchOnOff(true);
     Serial.println("Transmit ON/OFF done");
   }
   showLCD(lastTemp, lastHumi, state);

   //delay(1000);
  sendCount++;
  sendOnOff++;
  blinkLED();

  Serial.println("About to sleep...");
  delay(200);
  sleep.pwrDownMode(); //set sleep mode
  sleep.sleepDelay(sleepTime); //sleep for: sleepTime 10 seconds
}

