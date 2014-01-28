
//based on webClient_netio06.ino
//
//this code works in SIMULATION at HOME
//code also works with receiver (undef SIMULATION) attached at HOME

// sending (GET request) data to php/mysql server
/*
index.php?channel=1&temp=165&humidity=80&datetime=010120131234
only channel, temp and humidity are evaluated
*/

#include <EtherCard.h>

#include <SensorReceiver.h>
//#include <SensorTransmitter.h>
#include <InterruptChain.h>

// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

//v6 adding sensor code and interrupts

#define USEDNS
#undef USEDNS

#define USEDHCP
#undef USEDHCP

#define MYDEBUG
//#undef MYDEBUG

#define HOME
//#undef HOME

//if no receiver is attached
#define SIMULATION
#undef SIMULATION


#ifdef MYDEBUG
  long postinterval = 15 * 1000; //15 seconds
#else
  long postinterval = 5 * 60 * 1000; // 5 minutes
#endif

int LED = 14; //analog ADC3

bool usepersistentmode=false;

enum STATE{idle, waiting};
volatile STATE state=idle;  //0 = idle, 1 = waiting for response
char* str=(char*)malloc(80);

#ifdef HOME
  // ethernet interface mac address, must be unique on the LAN
  static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
  // ### for static IP ###
  // ethernet interface ip address
  static byte myip[] = { 192,168,0,41 };
  // gateway ip address
  static byte gwip[] = { 192,168,0,40 };
  // dns ip address
  static byte dns[] =  { 192,168,0,40 };
  // ### end for static IP ###
  #ifdef USEDNS
    char website[] PROGMEM = "www.google.com";   //works OK 9. jan 2013 HGO
    //char website[] PROGMEM = "192.168.128.5";      //does not work as a DNS query is used against website
    //char website[] PROGMEM = "smart.intermec.local";  //does not work with local hosts entry as DNS is used
  #else  //if you need to use a static web server and have no name server:
    char website[] PROGMEM = "atom2";  //use any name you want
    byte websiteIP[]={192,168,0,40};  //enter the IP to be used for  website
  #endif
#else
  // ethernet interface mac address, must be unique on the LAN
  static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
  // ### for static IP ###
  // ethernet interface ip address
  static byte myip[] = { 192,168,128,41 };
  // gateway ip address
  static byte gwip[] = { 192,168,128,5 };
  // dns ip address
  static byte dns[] =  { 192,168,128,5 };
  // ### end for static IP ###
  #ifdef USEDNS
    char website[] PROGMEM = "www.google.com";   //works OK 9. jan 2013 HGO
    //char website[] PROGMEM = "192.168.128.5";      //does not work as a DNS query is used against website
    //char website[] PROGMEM = "smart.intermec.local";  //does not work with local hosts entry as DNS is used
  #else  //if you need to use a static web server and have no name server:
    char website[] PROGMEM = "smart.local";  //use any name you want
    byte websiteIP[]={192,168,128,5};  //enter the IP to be used for  website
  #endif
#endif

#ifdef SIMULATION
  #include <TimerOne.h>
  volatile byte currentSimuChannel = 1;
#endif

byte Ethernet::buffer[700];
static uint32_t timer;

#ifndef time_t
  #define time_t long
#endif
time_t time_long;
#define MAX_CHANNEL 4  //browseUrl without function if MAX_CHANNEL = 5, driving me crazy!

//store last values
typedef struct {
    long time_long;    //store int time of data update
    byte bUpdated;
    byte channel;
    int temp;
    short int humidity;
}SENSORDATA;
volatile SENSORDATA sensorData[MAX_CHANNEL];

//############################################
// forward declares
//############################################
static void my_callback (byte status, word off, word len);
void updateSensorData(byte channel, int temp, byte humi);

//############################################
// blink LED for optical response
//############################################
void blinkLED(){
  static byte bState = HIGH;
    digitalWrite(LED, bState);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    if(bState==HIGH)
      bState=LOW;
    else
      bState=HIGH;
//    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
//    delay(1000);               // wait for a second
}

//############################################
// showTempHumi, interrupt callback!
// Show and update the received sensor data
//############################################
void showTempHumi(byte *data) {
  // is data a ThermoHygro-device?
  if ((data[3] & 0x1f) == 0x1e) {
    // Yes!
    byte channel, randomId;
    int temp;
    byte humidity;  //was ushort before

    // Decode the data
    SensorReceiver::decodeThermoHygro(data, channel, randomId, temp, humidity);
    //store data
    updateSensorData(channel, temp, humidity);
//  the following may crash inside this ISR function
/*
    // Print temperature. Note: temp is 10x the actual temperature!
    Serial.print("Temperature: ");
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
*/
  }
}

//############################################
//  setupReceiver inits the interrupts for 
//  433MHz OOK signaling
//############################################
void setupReceiver(){
  // And once more, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  SensorReceiver::init(0, showTempHumi); //0 is PD2=INT0

  // On interrupt, call the interrupt handlers of remote and sensor receivers
  //InterruptChain::addInterruptCallback(0, SensorReceiver::interruptHandler);
}

//############################################
//  dump the free RAM
//############################################
int freeRam () {
  // __brkval is the address of the top of the heap if memory has been allocated.
  // If __brkval is zero then it means malloc has not used any memory yet, so
  // we look at the address of __heap_start.
  extern int __heap_start;
  extern int *__brkval; // address of the top of heap
  int stack_top; 
  return (int)&stack_top - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval); 
}

//############################################
//  initData
//############################################
void initData(){
  for (int i=0; i<MAX_CHANNEL; i++){
    memset((void*)&sensorData[i], 0, sizeof(sensorData));
  }
}
//############################################
//  updateSensorData, for channel 1 to 3, idx=0 to 2
//############################################
void updateSensorData(byte channel, int temp, byte humi){
  blinkLED();
  int idxChannel = channel - 1;
  static time_t last_time=0;
  if(1<=channel && channel<=MAX_CHANNEL)
  {
    if(1){//bGotValidTime){
      //strcpy(sensorData[idxChannel].strDay, strDay);
      //strcpy(sensorData[idxChannel].strClock, strClock);
      sensorData[idxChannel].time_long = millis();
      last_time=time_long;
    }
    sensorData[idxChannel].time_long = millis();
    last_time = sensorData[idxChannel].time_long;
    sensorData[idxChannel].channel=channel;
    sensorData[idxChannel].temp=temp;    
    sensorData[idxChannel].humidity=humi;
    sensorData[idxChannel].bUpdated = 1;
  }
  blinkLED();
}

#ifdef SIMULATION
  //############################################
  /// Custom ISR Timer Routine to simulate incoming data updates
  //############################################
  void timerIsr()
  {
    //called every second
    blinkLED();
    static byte intCount=0; 
    intCount++;
    if(intCount>10){  //do all 10 seconds
      updateSensorData(currentSimuChannel, 11, 44);
      currentSimuChannel++;
      if(currentSimuChannel>MAX_CHANNEL)
        currentSimuChannel=1;
      intCount=0;
    }
  }
#endif

//############################################
//  sendData, channel=1 to 3
//############################################
void sendData(byte channel, int temp, byte humidity, long time_long){
  int idxChannel = channel - 1;
  /*
  if(sensorData[idxChannel].bUpdated ==0){
    Serial.print("channel "); Serial.print(channel); Serial.println(" not updated. No send. State now idle");
    state=idle;
    return;
  }
  */
  /*
  byte bchannel = sensorData[idxChannel].channel;
  int itemp = sensorData[idxChannel].temp;
  byte bhumi = sensorData[idxChannel].humidity;
  long ltime = sensorData[idxChannel].time_long;
  */
  byte bchannel = channel;
  int itemp = temp;
  byte bhumi = humidity;
  long ltime = time_long;
  
  //ether.browseUrl(PSTR("/foo/"), "bar", website, my_callback);  //original OK, except you get a 404 not found error
  //ether.browseUrl(PSTR("/homewatch/"), "index.php", website, my_callback); // works OK
  if(state==idle){ //idle??
    Serial.println("state=idle: browseUrl()...");
    EtherCard::persistTcpConnection(usepersistentmode);

  Serial.println("++++++++++++++++");
  memset(str, 0, 80);
  sprintf(str, "channel=%i&temp=%i&humidity=%i&time=%lu", bchannel, itemp, bhumi, ltime);
    ether.browseUrl(PSTR("/homewatch/index.php?"), str, website, my_callback); // works OK, sometimes
    /*
    "GET /homewatch/index.php?channel=1&temp=222&humidity=55&time=510171 HTTP/1.0" 200 2428 "-" "-    OK
    "GET /homewatch/index.php?channel 2 not updated. No send. State now idle HTTP/1.0" 200 2392 FALSE
    */    
  Serial.println(str);
  Serial.println("++++++++++++++++");

    sensorData[idxChannel].bUpdated = 0;
    ether.packetLoop(ether.packetReceive()); //discard further pakets
    //wait until response or timeout?
    state=waiting; //waiting
  }
  else if(state==waiting){ //state=waiting?
    Serial.println("state=waiting: browseUrl() skipped");
    ether.packetLoop(ether.packetReceive()); //discard further pakets
  }
}

//############################################
// mycallback, called when the client request is complete
//############################################
static void my_callback (byte status, word off, word len) {
  // Called for each packet of returned data from the call to browseUrl,
  // if persistent mode is set just before the call to browseUrl
  Serial.println(">>>");
  //Ethernet::buffer[off+len] = 0;  //get allmost all data back
  Ethernet::buffer[off+300] = 0;  //set a str end marker at offset 300
  Serial.println((const char*) Ethernet::buffer + off);
  Serial.println(">>> MyCallback done <<<");
  state=idle;
}

//############################################
// SETUP
//############################################
void setup () {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(LED, OUTPUT);     
  Serial.println("\n[webClient]");
  #ifdef MYDEBUG
    Serial.println("\nMYDEBUG version");
  #else
    Serial.println("\nrelease version");
  #endif
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, 28) == 0) //added 28 argument for netio HGO
    Serial.println( "Failed to access Ethernet controller" );
    
#ifdef USEDHCP  
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
  else{
    Serial.println("DHCP failed, using static IP");
    ether.staticSetup(myip, gwip, dns);
  }
#else
    Serial.println("USEDHCP undefined, using static IP");
    ether.staticSetup(myip, gwip, dns);
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

#ifdef USEDNS
  Serial.println("Waiting for linkUp...");
 // Wait for link to become up - this speeds up the dnsLoopup in the current version of the Ethercard library 
  while (!ether.isLinkUp()) { ether.packetLoop(ether.packetReceive()); }
  Serial.println("link detected.");
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
#endif    
  EtherCard::copyIp(ether.hisip, websiteIP);
  ether.printIp("SRV: ", ether.hisip);

  initData();
  
#ifndef SIMULATION
  Serial.println("setupReceiver()...");
  setupReceiver();
#else  //to simulate incoming data
  Serial.println("starting Timer1 to simulate data update...");
  Timer1.initialize(1000000); // set a timer of length 1000000 microseconds (or 1 sec)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
#endif

  state=idle; //idle
}

//############################################
// LOOP
//############################################
void loop () {
  static uint32_t timer2;
  static byte idx=1;
  ether.packetLoop(ether.packetReceive()); // Need to flush the received packet to clear the last transfer
  if (millis() > timer) {
    timer = millis() + postinterval;  //15 seconds or 5 minutes
    Serial.println("timer1 elapsed");
    //updateSensorData(idx, 222, 55);
    for(int i=1; i<=MAX_CHANNEL; i++){
      int idxChannel=i-1;
      Serial.println();
      Serial.print("senddata(");Serial.print(i);Serial.println(")?");

      if(sensorData[i-1].bUpdated ==0){
        Serial.print("channel "); Serial.print(i); Serial.println(" not updated. No send. State now idle");
        state=idle;
      }
      else{
        sendData(i, 
          sensorData[idxChannel].temp, 
          sensorData[idxChannel].humidity,
          sensorData[idxChannel].time_long);
        ether.packetLoop(ether.packetReceive()); // Need to flush the received packet to clear the last transfer
      }
    }
    
    idx++;
    if(idx>3)
      idx=1;
  }
  if(millis() > timer2){
    timer2 = millis() + 10000;// every xx secs
    if(state==waiting){
        Serial.println("######## waiting aborted #########");
        state=idle;
    }
  }
}

