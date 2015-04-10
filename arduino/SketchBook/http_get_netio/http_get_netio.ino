// Simple demo for feeding some random data to Pachube.
// 2011-07-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
#define MYDEBUG 1

#include <Time.h>
#include <SensorReceiver.h>
#include <InterruptChain.h>

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
char website[] PROGMEM = "atom2";
byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;

char bufA[80];
char* buf=&bufA[0];// "                     ";
time_t time_long;  
time_t lastConnectionTime;
#if MYDEBUG
const unsigned long postingInterval = 10; // delay between updates to website in seconds
#else
const unsigned long postingInterval = 5 * 60; // delay between updates to website in seconds
#endif

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

//forward declares
char* printTime(char* str, int len, time_t tTime);

int freeRam () {
  // __brkval is the address of the top of the heap if memory has been allocated.
  // If __brkval is zero then it means malloc has not used any memory yet, so
  // we look at the address of __heap_start.
  extern int __heap_start;
  extern int *__brkval; // address of the top of heap
  int stack_top; 
  return (int)&stack_top - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval); 
}

// index.php?channel=1&temp=165&humidity=80&datetime=010120131234
//###################################################################
char* printDateTime(char* str, int len, time_t tTime){
  char timestr[] = "%02i%02i%04i%02i%02i%02i\0";
  snprintf(str, len, timestr, 
    day(tTime),
    month(tTime),
    year(tTime),
    hour(tTime),
    minute(tTime),
    second(tTime));

  Serial.println(F("printDateTime: "));
  Serial.println(str);
  Serial.println(F("####"));
  return str;
}

//###################################################################################
//updateSensorData array
void updateSensorData(byte channel, int temp, short int humi){
  static time_t last_time=0;
  static word old_millis=millis();
  int idxChannel = channel - 1;
  if(1<=channel && channel<=MAX_CHANNEL)
  {
    sensorData[idxChannel].time_long = time_long;
    last_time=time_long;
    
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
  // And once more, interrupt -1 to indicate you will call the interrupt handler with InterruptChain
  SensorReceiver::init(-1, showTempHumi);
  // On interrupt, call the interrupt handlers of remote and sensor receivers
  InterruptChain::addInterruptCallback(0, SensorReceiver::interruptHandler);
}

//###################################
//#             INIT data           #
//###################################
void initData(){
  for (int i=0; i<MAX_CHANNEL; i++){
    memset((void*)&sensorData[i], 0, sizeof(sensorData));
  }
}

void sendData(int idxChannel){
    Serial.print(F("in sendData for idx=")); Serial.println(idxChannel);
    byte sd;
    int channel=sensorData[idxChannel].channel;
    int temp=sensorData[idxChannel].temp;
    int humidity=sensorData[idxChannel].humidity;
    char* stime="201301011122";
    stime=printDateTime((char*)&stime, 13, sensorData[idxChannel].time_long);
    
    if(sensorData[idxChannel].bUpdated==0){
      //nothing new
      if (MYDEBUG==0){
        Serial.println(F("leaving for not updated channel data"));
        goto exit_sendData;
      }
      else
      {
        sensorData[idxChannel].channel=idxChannel;
        sensorData[idxChannel].temp=222;
        sensorData[idxChannel].humidity=55;
        sensorData[idxChannel].time_long=now();      
      }
    }

    // generate two fake values as payload - by using a separate stash,
    // we can determine the size of the generated message ahead of time
    sd = stash.create();
    stash.print("0,");
    stash.println((word) millis() / 123);
    stash.print("1,");
    stash.println((word) micros() / 456);
    stash.save();
    
    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("GET http://$F/homewatch/index.php?channel=$D&temp=$D&humidity=$D&time=$S" "\r\n"
                        "Host: $F" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
            website, 
            channel,
            temp,
            humidity,
            stime,
            website, stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();
exit_sendData:
  lastConnectionTime = now();//millis();
  Serial.println(F("...end of sendData()"));
}

//###################################################################
int timeDiff(time_t start, time_t ende){
  unsigned long startTimeSeconds = hour(start)*60*60 + minute(start)*60 + second(start);
  unsigned long endTimeSeconds = hour(ende)*60*60 + minute(ende)*60 + second(ende);
  int iDiffSeconds = abs(abs(endTimeSeconds) - abs(startTimeSeconds)); 
  return iDiffSeconds;
}

//###################################
//#                SETUP            #
//###################################
void setup () {
  Serial.begin(57600);
  Serial.println("\n[webClient]");

  if (ether.begin(sizeof Ethernet::buffer, mymac, 28) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  byte serverip[] = { 192,168,0,40 };
  ether.copyIp(ether.hisip, serverip);
  ether.printIp("SRV: ", ether.hisip);
  
    //print start time
  time_t currentTime=now();
  setTime(currentTime);  
  printDateTime(buf, 19, currentTime);
  initData();
  setupReceiver();
  
  Serial.println(F("Setup done."));
}

//###################################
//#            LOOP                 #
//###################################
void loop () {
  ether.packetLoop(ether.packetReceive());
  time_t currentTime = now();
  Serial.print(F("+++ Timediff: "));
  Serial.println(timeDiff(lastConnectionTime, currentTime));
  printDateTime(buf, 13, currentTime);

//  if(!client.connected() && ((millis() - lastConnectionTime) > postingInterval)) {
//  if(!client.connected() && (timeDiff(lastConnectionTime, currentTime) >= postingInterval)) {
  if((timeDiff(lastConnectionTime, currentTime) >= postingInterval)) {
    for(int i=0; i<MAX_CHANNEL; i++)
      sendData(i);
    Serial.print("Free RAM: ");
    Serial.println(freeRam());
  }
  delay(1000);
}
