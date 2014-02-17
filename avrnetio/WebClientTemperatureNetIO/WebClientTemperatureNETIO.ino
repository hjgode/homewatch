#include <Time.h>
//#include "/usr/local/include/libavutil/time.h"

#include <avr/wdt.h>  //watchdog!
#include <RemoteReceiver.h>
#include <NewRemoteReceiver.h>
#include <SensorReceiver.h>
//#include <SensorTransmitter.h>
#include <InterruptChain.h>

#include <SPI.h>

//#include <Ethernet.h>
#include <EtherCard.h>
#include <enc28j60.h>
#include <net.h>
Stash stash;
BufferFiller bfill;

//a global buffer for snprintf
const size_t maxBuf=254;
char cBuf[maxBuf+1];
const char getHttpString[] = 
  "GET /homewatch/index.php?channel=%i&temp=%i&humidity=%i&time=%i\0";

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
static byte mymac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte myserver[] = { 192,168,0,40 }; // Google
// fill in an available IP address on your network here,
// for manual configuration:
static byte myip[] = { 192,168,0,199 };
// gateway ip address
static byte gwip[] = { 192,168,0,250 };

//need a buffer for browseUrl callback
byte Ethernet::buffer[700]; // a very small tcp/ip buffer is enough here

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
//EthernetClient client;

//unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
time_t lastConnectionTime;
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 5 * 60; // delay between updates to website in seconds

/*
index.php?channel=1&temp=165&humidity=80&datetime=010120131234

only channel, temp and humidity are evaluated
*/
int freeRam () {
  // __brkval is the address of the top of the heap if memory has been allocated.
  // If __brkval is zero then it means malloc has not used any memory yet, so
  // we look at the address of __heap_start.
  extern int __heap_start;
  extern int *__brkval; // address of the top of heap
  int stack_top; 
  return (int)&stack_top - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval); 
}


//store last values
typedef struct {
    time_t time_long;    //store int time of data update
    byte bUpdated;
    byte channel;
    int temp;
    short int humidity;
}SENSORDATA;

#define MAX_CHANNEL 3
volatile SENSORDATA sensorData[MAX_CHANNEL];

  char strDay[22];        //string to hold day
  char strClock[22];      //string to hold clock
  time_t time_long;
  boolean bGotValidTime=false;
  void getNTPtime();  // forward declaration

//###################################################################################
// shows the received code sent from an old-style remote switch
void showOldCode(unsigned long receivedCode, unsigned int period) {
  // Print the received code.
  Serial.print("Code: ");
  Serial.print(receivedCode);
  Serial.print(", period: ");
  Serial.print(period);
  Serial.println("us.");
}

//###################################################################################
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

//###################################################################################
static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RBBB server</title>" 
    "<h1>$D$D:$D$D:$D$D</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

void doWebServer(){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
}
//###################################################################################
//updateSensorData array
void updateSensorData(byte channel, int temp, short int humi){
  static time_t last_time=0;
  static word old_millis=millis();
  int idxChannel = channel - 1;
  if(1<=channel && channel<=MAX_CHANNEL)
  {
    if(bGotValidTime){
      //strcpy(sensorData[idxChannel].strDay, strDay);
      //strcpy(sensorData[idxChannel].strClock, strClock);
      sensorData[idxChannel].time_long = time_long;
      last_time=time_long;
    }
    else{
      //sprintf(sensorData[idxChannel].strDay, "-");
      //sprintf(sensorData[idxChannel].strClock,"-");
    }
    sensorData[idxChannel].time_long = now();
    last_time = sensorData[idxChannel].time_long;
    sensorData[idxChannel].channel=channel;
    sensorData[idxChannel].temp=temp;    
    sensorData[idxChannel].humidity=humi;
    sensorData[idxChannel].bUpdated = 1;
  }
}

//###################################################################################
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
  
//###################################################################################
  //store data
  updateSensorData(channel, temp, humidity);
  
    // Print temperature. Note: temp is 10x the actual temperature!
    Serial.print(F("Temperature: "));
    Serial.print(temp / 10); // units
    Serial.print('.');
    Serial.print(temp % 10); // decimal
    
        // Print humidity
    Serial.print(F(" deg, Humidity: "));
    Serial.print(humidity);
    Serial.print(F("% REL"));

    // Print channel
    Serial.print(F(", Channel: "));
    Serial.println(channel, DEC); 
  }
}
//###################################################################################
void setupReceiver(){
  // Interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  RemoteReceiver::init(-1, 2, showOldCode);
  
  // Again, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  NewRemoteReceiver::init(-1, 2, showNewCode);

  // And once more, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  SensorReceiver::init(-1, showTempHumi);

  // On interrupt, call the interrupt handlers of remote and sensor receivers
  InterruptChain::addInterruptCallback(0, RemoteReceiver::interruptHandler);
  InterruptChain::addInterruptCallback(0, NewRemoteReceiver::interruptHandler);
  InterruptChain::addInterruptCallback(0, SensorReceiver::interruptHandler);

}

void initData(){
  for (int i=0; i<MAX_CHANNEL; i++){
    memset((void*)&sensorData[i], 0, sizeof(sensorData));
  }
}

void getNTPtime(){
  bGotValidTime=false;
}


//##########################################################################
//char website[] PROGMEM = "atom2";
char website[] PROGMEM = "192.168.0.40";
static byte serverIp[] = { 192,168,0,40 };
static byte session_id;
//##########################################################################
static void response_callback (byte status, word off, word len) 
{ 
  Serial.print((const char*) Ethernet::buffer + off);
}
void checkResponse() {
  const char* reply = ether.tcpReply(session_id);
  if(reply > 0) {
    if(strstr(reply, "KO - ") != 0) 
      Serial.println(strstr(reply, "KO - "));
    else 
      Serial.println("OK");
  }
}
//##########################################################################
void sendData(int idxChannel){
  Serial.print(F("in sendData for idx=")); Serial.println(idxChannel);
  if(sensorData[idxChannel].bUpdated==0){
    //nothing new
    Serial.println(F("leaving for not updated channel data"));
    goto exit_sendData;
  }
  // and send data: /index.php?channel=1&temp=165&humidity=80&datetime=010120131234
  
  //if (client.connect(server, 80)) {
  if(true){
    Serial.println("connected. Send GET...");
    snprintf(cBuf, maxBuf, 
      getHttpString, 
      sensorData[idxChannel].channel,
      sensorData[idxChannel].temp,
      sensorData[idxChannel].humidity,
      sensorData[idxChannel].time_long
      );

Serial.println(F("####"));
Serial.println(cBuf);
Serial.println(F("####"));

    //see http://www.lucadentella.it/en/2012/02/19/enc28j60-e-arduino-3/2/
    //and https://github.com/lucadentella/enc28j60_tutorial/blob/master/_14_WebTemperature/_14_WebTemperature.ino
    ether.copyIp(ether.hisip,serverIp);
//+++++++ THIS +++++
//    ether.browseUrl(PSTR("/homewatch/"), "index.php?channel=1&temp=165&humidity=80&datetime=010120131234", website, response_callback);
//+++++++ OR +++++
    Stash stash;
    byte sd = stash.create();    
    stash.print(cBuf);
    stash.save();
    //cBuf =   "GET /homewatch/index.php?channel=%i&temp=%i&humidity=%i&time=%i\0"  
    Stash::prepare(PSTR("$H HTTP/1.0" "\r\n"
      "Host: $F" "\r\n" "\r\n"),
      sd, website);
    session_id = ether.tcpSend();
    ether.packetLoop(ether.packetReceive());
    checkResponse();
//-------------    
    //client.println(cBuf);
    
    sensorData[idxChannel].bUpdated=0;    //data has send
    Serial.print(F("updated=0 for channel: ")); 
    Serial.println(sensorData[idxChannel].channel);
  } 
  else{
    // if you couldn't make a connection:
    Serial.println(F("connection failed"));
/*    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
*/
  }
exit_sendData:
  ether.packetLoop(ether.packetReceive());
   // note the time that the connection was made or attempted:
  lastConnectionTime = now();//millis();
  Serial.println(F("...end of sendData()"));
}

/*
  homewatch client
*/

//###################################################################
void printTime(time_t tTime){
  char timestr[] = "int Time: %02i:%02i:%02i\0";
  snprintf(cBuf, maxBuf, timestr, 
    hour(tTime),
    minute(tTime),
    second(tTime));

  Serial.println(F("####"));
  Serial.println(cBuf);
  Serial.println(F("####"));
}

//###################################################################
int timeDiff(time_t start, time_t ende){
  unsigned long startTimeSeconds = hour(start)*60*60 + minute(start)*60 + second(start);
  unsigned long endTimeSeconds = hour(ende)*60*60 + minute(ende)*60 + second(ende);
  int iDiffSeconds = abs(abs(endTimeSeconds) - abs(startTimeSeconds)); 
  return iDiffSeconds;
}

//#########################

//###################################################################
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println(F("Starting homewatch client 0.01"));

  // start the Ethernet connection:
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Failed to access Ethernet controller");
  ether.staticSetup(myip, gwip);
  ether.copyIp(ether.hisip, myserver);

  ether.printIp("IP: ", ether.myip);
  ether.printIp("GW: ", ether.gwip);
  ether.printIp("Server: ", ether.hisip);

  // give the Ethernet shield a second to initialize:
  delay(1000);
  
  //enable watchdog for 8s interval
  wdt_enable(WDTO_8S);

  //print start time
  time_t currentTime=now();
  setTime(currentTime);  
  printTime(currentTime);
  initData();
  setupReceiver();
  
  Serial.println(F("Setup done."));
}

void loop()
{
  //keep ethernet running!
  ether.packetLoop(ether.packetReceive());
//  Serial.println("Loop start...");
/*
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
*/
  wdt_reset();
/*
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();
  }
*/
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  time_t currentTime = now();
  Serial.print(F("+++ Timediff: "));
  Serial.println(timeDiff(lastConnectionTime, currentTime));
  printTime(currentTime);

//  if(!client.connected() && ((millis() - lastConnectionTime) > postingInterval)) {
//  if(!client.connected() && (timeDiff(lastConnectionTime, currentTime) >= postingInterval)) {
  if((timeDiff(lastConnectionTime, currentTime) >= postingInterval)) {
    for(int i=0; i<MAX_CHANNEL; i++)
      sendData(i);
    Serial.print("Free RAM: ");
    Serial.println(freeRam());
  }
  // store the state of the connection for next time through
  // the loop:
//  lastConnected = client.connected();
  //Serial.println("...loop end.");
  delay(1000);
  //doWebServer();
}

    /*
    ether.packetLoop(ether.packetReceive());
    //...
    byte sd = stash.create();
    stash.print("0,");
    stash.println((word) millis() / 123);
    stash.print("1,");
    stash.println((word) micros() / 456);
    stash.save();    
    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "X-PachubeApiKey: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
    website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);
    
    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();
    
    //#################################
    //see http://arduino-life.blogspot.de/
     byte sd = stash.create();
     stash.print("GET /heatmiser/update_temp.php?");
            stash.print("Office=");
            stash.print(tempOffice);
            stash.print("&Outside=");
            stash.print(tempOutside);
            stash.print("&Master=");
            stash.print(tempMaster);
            stash.print("&Howard=");
            stash.print(tempHoward);
            stash.print(" HTTP/1.0 \r\n");
     stash.println("\r\n");
     stash.save();
     Stash::prepare(PSTR("$H"), sd);
     ether.tcpSend();
    
    */
    /*
    or simply use browseURL, see http://blog.thiseldo.co.uk/?p=623 and http://www.rogerclark.net/?p=487
    and http://pastebin.com/Uq5DMaXg
    ...
    void browserresult_callback(byte success,word data, word dataLen)
    {
        Serial.println("callback called");
        //Serial.println( (char*)data );
    }
    ...
    ether.browseUrl(PSTR("/xyz.php"), "?search=Arduino", PSTR(HOSTNAME), &browserresult_callback);
    */
    /*
    byte sd = stash.create();
    stash.print(cBuf);
    stash.println("\r\n");
    stash.save();
    Stash::prepare(PSTR("$H"), sd);
    ether.tcpSend();
    */

