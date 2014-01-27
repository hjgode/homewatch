#define USE_INET

/*
 * This sketch demonstrates how to use InterruptChain to receive and
 * decode remote switches (old and new) and remote sensors.
 *
 * Basically, this sketch combines the features of the ShowReceivedCode
 * and ShowReceivedCodeNewKaku examples of RemoteSwitch and the
 * ThermoHygroReceiver of RemoteSensor all at the same time!
 *
 * After uploading, enable the serial monitor at 115200 baud.
 * When you press buttons on a 433MHz remote control, as supported by 
 * RemoteSwitch or NewRemoteSwitch, the code will be echoed.
 * At the same time, if data of a remote thermo/hygro-sensor is
 * received, as supported by RemoteSensor, it will be echoed as well.
 *
 * Setup:
 * - connect a 433MHz receiver on digital pin 2.
 */

#include <RemoteReceiver.h>
#include <NewRemoteReceiver.h>
#include <SensorReceiver.h>
#include <InterruptChain.h>

//store last values
typedef struct {
    char strDay[22];
    char strClock[22];
    uint32_t time_long;
    byte channel;
    int temp;
    short int humidity;
}SENSORDATA;

#define MAX_CHANNEL 3
SENSORDATA sensorData[MAX_CHANNEL];

#ifdef USE_INET
  #include <EtherCard.h>
  
//  unsigned long getNtpTime();  // forward declaration
  char strDay[22];        //string to hold day
  char strClock[22];      //string to hold clock
  uint32_t time_long;
  boolean bGotValidTime;
  void getNTPtime();  // forward declaration
  
  // ethernet interface mac address, must be unique on the LAN
  static byte mymac[] = { 0x00,0x22,0xf9,0x01,0xb2,0x28 };
  static byte myip[] = { 192,168,0,13 };
  static byte mymask[] = { 255,255,255,0 };
  static byte mygw[]   = { 192,168,0,250 };
  
  byte Ethernet::buffer[500];
  BufferFiller bfill;
  
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
      //"<meta http-equiv='refresh' content='60'/>"
      "<title>Sensor server</title>" 
      "<h1>$D$D:$D$D:$D$D</h1>"),
        h/10, h%10, m/10, m%10, s/10, s%10);

    bfill.emit_p(PSTR("<p><table>"
       "<tr><td>Sensor</td><td>Temp C</td><td>Humidity %</td><td>Timestamp</td></tr>\r\n"));
    for(int i=1; i<=MAX_CHANNEL; i++){
      bfill.emit_p(PSTR(
      "<tr><td>$D</td><td>$D.$D</td><td>$D</td><td>$S $S</td></tr>"),
      i, sensorData[i-1].temp / 10, sensorData[i-1].temp % 10, sensorData[i-1].humidity,
      sensorData[i-1].strDay, sensorData[i-1].strClock
      );
    }    
    bfill.emit_p(PSTR("</table>"));
    return bfill.position();
  }
#endif


/*
void showOldCode(unsigned long receivedCode, unsigned int period);
void showNewCode(NewRemoteCode receivedCode);
void showTempHumi(byte *data);
*/
void setup() {
  Serial.begin(115200);

  Serial.println("http://randysimons.nl/");

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

#ifdef USE_INET  
  //ethernet
  if (ether.begin(sizeof Ethernet::buffer, mymac,28) == 0)
    Serial.println( "Failed to access Ethernet controller");
  else
    Serial.println( "Listening on 192.168.0.13");
  ether.staticSetup(myip, mygw, mygw);
  ether.copyIp(ether.mymask,mymask);

  Serial.println("ntpTime...");
//  unsigned long ntpTime = getNtpTime();
//  Serial.print(ntpTime);
//  Serial.println("");
  ntpSetup();
#endif //USE_INET

}

void loop() {
   // You can do other stuff here!
#ifdef USE_INET   
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
  
  getNTPtime();
  //delay(5000);
#endif
}

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

//##############################################################
//https://github.com/thiseldo/EtherCardExamples/blob/master/EtherCard_ntp/EtherCard_ntp.ino
static int currentTimeserver = 0;

// Find list of servers at http://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
// Please observe server restrictions with regard to access to these servers.
// This number should match how many ntp time server strings we have
#define NUM_TIMESERVERS 5

// Create an entry for each timeserver to use
prog_char ntp0[] PROGMEM = "ntp2d.mcc.ac.uk";
prog_char ntp1[] PROGMEM = "ntp2c.mcc.ac.uk";
prog_char ntp2[] PROGMEM = "ntp.exnet.com";
prog_char ntp3[] PROGMEM = "ntp.cis.strath.ac.uk";
prog_char ntp4[] PROGMEM = "clock01.mnuk01.burstnet.eu";

// Now define another array in PROGMEM for the above strings
prog_char *ntpList[] PROGMEM = { ntp0, ntp1, ntp2, ntp3, ntp4 };

uint8_t clientPort = 123;

// The next part is to deal with converting time received from NTP servers
// to a value that can be displayed. This code was taken from somewhere that
// I cant remember. Apologies for no acknowledgement.

uint32_t lastUpdate = 0;
uint32_t timeLong;
// Number of seconds between 1-Jan-1900 and 1-Jan-1970, unix time starts 1970
// and ntp time starts 1900.
#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL

#define EPOCH_YR 1970
//(24L * 60L * 60L)
#define SECS_DAY 86400UL
#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)

static const char day_abbrev[] PROGMEM = "SunMonTueWedThuFriSat";
// isleapyear = 0-1
// month=0-11
// return: how many days a month has
//
// We could do this if ram was no issue:

uint8_t monthlen(uint8_t isleapyear,uint8_t month){
  const uint8_t mlen[2][12] = {
    {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    ,
    {
      31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
  };
  return(mlen[isleapyear][month]);
}

// gmtime -- convert calendar time (sec since 1970) into broken down time
// returns something like Fri 2007-10-19 in day and 01:02:21 in clock
// The return values is the minutes as integer. This way you can update
// the entire display when the minutes have changed and otherwise just
// write current time (clock). That way an LCD display needs complete
// re-write only every minute.
uint8_t gmtime(const uint32_t time,char *day, char *clock)
{
  char dstr[4];
  uint8_t i;
  uint32_t dayclock;
  uint16_t dayno;
  uint16_t tm_year = EPOCH_YR;
  uint8_t tm_sec,tm_min,tm_hour,tm_wday,tm_mon;

  dayclock = time % SECS_DAY;
  dayno = time / SECS_DAY;

  tm_sec = dayclock % 60UL;
  tm_min = (dayclock % 3600UL) / 60;
  tm_hour = dayclock / 3600UL;
  tm_wday = (dayno + 4) % 7; /* day 0 was a thursday */
  while (dayno >= YEARSIZE(tm_year)) {
    dayno -= YEARSIZE(tm_year);
    tm_year++;
  }
  tm_mon = 0;
  while (dayno >= monthlen(LEAPYEAR(tm_year),tm_mon)) {
    dayno -= monthlen(LEAPYEAR(tm_year),tm_mon);
    tm_mon++;
  }
  i=0;
  while (i<3){
    dstr[i]= pgm_read_byte(&(day_abbrev[tm_wday*3 + i]));
    i++;
  }
  dstr[3]='\0';
  sprintf_P(day,PSTR("%s %u-%02u-%02u"),dstr,tm_year,tm_mon+1,dayno + 1);
  sprintf_P(clock,PSTR("%02u:%02u:%02u"),tm_hour+1,tm_min,tm_sec); // +1 for GMT+1
  return(tm_min);
}

void ntpSetup(){
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  lastUpdate = millis();
}

void getNTPtime(){
    uint16_t dat_p;
  char day[22];
  char clock[22];
  int plen = 0;
  
  // Main processing loop now we have our addresses
    // handle ping and wait for a tcp packet
    plen = ether.packetReceive();
    dat_p=ether.packetLoop(plen);
    // Has unprocessed packet response
    if (plen > 0) {
      timeLong = 0L;
     
      if (ether.ntpProcessAnswer(&timeLong,clientPort)) {
        Serial.print( F( "Time:" ));
        Serial.println(timeLong); // secs since year 1900
       
        if (timeLong) {
          timeLong -= GETTIMEOFDAY_TO_NTP_OFFSET;
          gmtime(timeLong,day,clock);
          strcpy(strDay, day);
          strcpy(strClock, clock);
          time_long=timeLong;
          bGotValidTime=true;
          Serial.print( day );
          Serial.print( " " );
          Serial.println( clock );
        }
        else
          bGotValidTime=false;
      }
    }
    // Request an update every 20s
    if( lastUpdate + 20000L < millis() ) {
      // time to send request
      lastUpdate = millis();
      Serial.print( F("TimeSvr: " ) );
      Serial.println( currentTimeserver, DEC );

      if (!ether.dnsLookup( (char*)pgm_read_word(&(ntpList[currentTimeserver])) )) {
        Serial.println( F("DNS failed" ));
      } else {
        ether.printIp("SRV: ", ether.hisip);
        
        Serial.print( F("Send NTP request " ));
        Serial.println( currentTimeserver, DEC );
      
        ether.ntpRequest(ether.hisip, ++clientPort);
        Serial.print( F("clientPort: "));
        Serial.println(clientPort, DEC );
      }
      if( ++currentTimeserver >= NUM_TIMESERVERS )
        currentTimeserver = 0;
    }
}
