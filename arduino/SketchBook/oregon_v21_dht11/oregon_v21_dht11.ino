//#define DEBUG

#include <dht11.h>

dht11 DHT11;

#define DHT11PIN 2

/*
 * connectingStuff, Oregon Scientific v2.1 Emitter
 * http://www.connectingstuff.net/blog/encodage-protocoles-oregon-scientific-sur-arduino/
 *
 * Copyright (C) 2013 olivier.lebrun@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

const byte TX_PIN = 4; //13; //PA0; //d 22;//4;
const byte LED_PIN = 13;

const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;

#define SEND_HIGH() digitalWrite(TX_PIN, HIGH)
#define SEND_LOW() digitalWrite(TX_PIN, LOW)

//temperatur only transmitter: THN132N => type id:EC40
//#define THN132N

//in contrast to THGR2228N

//temperature and humidity sensor THGN123N => type id=1D20 

#ifdef THN132N
  byte TYPE_ID[] = {0xEC,0x40};
  //byte TYPE_ID[] = {0x4C, 0xEA};
  //gives fhem OREGON: ERROR: Unknown sensor_id=ec40 bits=64 message='40EC4020BB2C11C003'
  // OREGON: ERROR: Unknown sensor_id=ea4c bits=64 message='40EA4C20BB2C116004'
  // OREGON: ERROR: Unknown sensor_id=4cea bits=64 message='404CEA20BB2C116004'
/* FHEM/41_OREGON.pm:
   ...
   OREGON_type_length_key(0xea4c, 68) =>
   {
    part => 'THN132N', checksum => \&OREGON_checksum1, method => \&OREGON_common_temp,
   },
   ...
   # THGN122N,THGR122NX,THGR228N,THGR268
   OREGON_type_length_key(0x1a2d, 80) =>
   {
    part => 'THGR228N', checksum => \&OREGON_checksum2, method => \&OREGON_common_temphydro,
   },

*/
#else
  byte TYPE_ID[] = {0x1A,0x2D};
/* fhem:
2015.04.08 19:43:46 5: myFHEMduino: 501A2D20BB2C1120053F00
2015.04.08 19:43:46 4: Dispatching OREGON Protokoll. Received: 501A2D20BB2C1120053F00
2015.04.08 19:43:46 5: myFHEMduino dispatch 501A2D20BB2C1120053F00
2015.04.08 19:43:46 5: FingerprintFn Message: Name: myFHEMduino  und Message: 501A2D20BB2C1120053F00
2015.04.08 19:43:46 5: OREGON: decoding delay=0 hex=501A2D20BB2C1120053F00
2015.04.08 19:43:46 5: Triggering THGR228N_bb_2 (4 changes)
2015.04.08 19:43:46 5: Notify loop for THGR228N_bb_2 temperature: 11.2
*/
#endif

// Buffer for Oregon message
#ifdef THN132N
  byte OregonMessageBuffer[8];
#else
  byte OregonMessageBuffer[9];
#endif

/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void sendZero(void) 
{
  SEND_HIGH();
  delayMicroseconds(TIME);
  SEND_LOW();
  delayMicroseconds(TWOTIME);
  SEND_HIGH();
  delayMicroseconds(TIME);
}

/**
 * \brief    Send logical "1" over RF
 * \details  a one bit be represented by an on-to-off transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void sendOne(void) 
{
   SEND_LOW();
   delayMicroseconds(TIME);
   SEND_HIGH();
   delayMicroseconds(TWOTIME);
   SEND_LOW();
   delayMicroseconds(TIME);
}

/**
* Send a bits quarter (4 bits = MSB from 8 bits value) over RF
*
* @param data Source data to process and sent
*/

/**
 * \brief    Send a bits quarter (4 bits = MSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void sendQuarterMSB(const byte data) 
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
}

/**
 * \brief    Send a bits quarter (4 bits = LSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void sendQuarterLSB(const byte data) 
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void sendData(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}

/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void sendOregon(byte *data, byte size)
{
    sendPreamble();
    //sendSync();
    sendData(data, size);
    sendPostamble();
}

/**
 * \brief    Send preamble
 * \details  The preamble consists of 16 "1" bits
 */
inline void sendPreamble(void)
{
  byte PREAMBLE[]={0xFF,0xFF};
  sendData(PREAMBLE, 2);
}

/**
 * \brief    Send postamble
 * \details  The postamble consists of 8 "0" bits
 */
inline void sendPostamble(void)
{
#ifdef THN132N
  sendQuarterLSB(0x00);
#else
  byte POSTAMBLE[]={0x00};
  sendData(POSTAMBLE, 1);  
#endif
}

/**
 * \brief    Send sync nibble
 * \details  The sync is 0xA. It is not use in this version since the sync nibble
 * \         is include in the Oregon message to send.
 */
inline void sendSync(void)
{
  sendQuarterLSB(0xA);
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

/**
 * \brief    Set the sensor type
 * \param    data       Oregon message
 * \param    type       Sensor type
 */
inline void setType(byte *data, byte* type) 
{
  data[0] = type[0];
  data[1] = type[1];
}

/**
 * \brief    Set the sensor channel
 * \param    data       Oregon message
 * \param    channel    Sensor channel (0x10, 0x20, 0x30)
 */
inline void setChannel(byte *data, byte channel) 
{
    data[2] = channel;
}

/**
 * \brief    Set the sensor ID
 * \param    data       Oregon message
 * \param    ID         Sensor unique ID
 */
inline void setId(byte *data, byte ID) 
{
  data[3] = ID;
}

/**
 * \brief    Set the sensor battery level
 * \param    data       Oregon message
 * \param    level      Battery level (0 = low, 1 = high)
 */
void setBatteryLevel(byte *data, byte level)
{
  if(!level) data[4] = 0x0C;
  else data[4] = 0x00;
}

/**
 * \brief    Set the sensor temperature
 * \param    data       Oregon message
 * \param    temp       the temperature
 */
void setTemperature(byte *data, float temp) 
{
  // Set temperature sign
  if(temp < 0)
  {
    data[6] = 0x08;
    temp *= -1;  
  }
  else
  {
    data[6] = 0x00;
  }

  // Determine decimal and float part
  int tempInt = (int)temp;
  int td = (int)(tempInt / 10);
  int tf = (int)round((float)((float)tempInt/10 - (float)td) * 10);

  int tempFloat =  (int)round((float)(temp - (float)tempInt) * 10);

  // Set temperature decimal part
  data[5] = (td << 4);
  data[5] |= tf;

  // Set temperature float part
  data[4] |= (tempFloat << 4);
}

/**
 * \brief    Set the sensor humidity
 * \param    data       Oregon message
 * \param    hum        the humidity
 */
void setHumidity(byte* data, byte hum)
{
    data[7] = (hum/10);
    data[6] |= (hum - data[7]*10) << 4;
}

/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */
int Sum(byte count, const byte* data)
{
  int s = 0;

  for(byte i = 0; i<count;i++)
  {
    s += (data[i]&0xF0) >> 4;
    s += (data[i]&0xF);
  }

  if(int(count) != count)
    s += (data[count]&0xF0) >> 4;

  return s;
}

/**
 * \brief    Calculate checksum
 * \param    data       Oregon message
 */
void calculateAndSetChecksum(byte* data)
{
#ifdef THN132N
    int s = ((Sum(6, data) + (data[6]&0xF) - 0xa) & 0xff);

    data[6] |=  (s&0x0F) << 4;     data[7] =  (s&0xF0) >> 4;
#else
    data[8] = ((Sum(8, data) - 0xa) & 0xFF);
#endif
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(TX_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("\n[Oregon V2.1 encoder]");

  SEND_LOW();  
  
  setType(OregonMessageBuffer, TYPE_ID);
  setChannel(OregonMessageBuffer, 0x20);
  setId(OregonMessageBuffer, 0xBB);
}

void loop()
{
  // Get Temperature, humidity and battery level from sensors
  // (ie: 1wire DS18B20 for température, ...)
  int chk = DHT11.read(DHT11PIN);
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
  }
  if(chk!=DHTLIB_OK)
    goto dht_error;
  
  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT11.temperature, 2);
  
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  setBatteryLevel(OregonMessageBuffer, 1); // 0 : low, 1 : high
  //setTemperature(OregonMessageBuffer, 11.2);
  setTemperature(OregonMessageBuffer, (float)DHT11.temperature);
  
#ifndef THN132N
  // Set Humidity
  //setHumidity(OregonMessageBuffer, 52);
  setHumidity(OregonMessageBuffer, (int)DHT11.humidity);
#endif  

  // Calculate the checksum
  calculateAndSetChecksum(OregonMessageBuffer);

  Serial.print("\n");
  // Show the Oregon Message
  for (byte i = 0; i < sizeof(OregonMessageBuffer); ++i)   {
    Serial.print(OregonMessageBuffer[i] >> 4, HEX);
    Serial.print(OregonMessageBuffer[i] & 0x0F, HEX);
  }

  Serial.print("Send Oriegon v2.1 message...\n");

  // Send the Message over RF
  sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));
  // Send a "pause"
  SEND_LOW();
  delayMicroseconds(TWOTIME*8);
  // Send a copie of the first message. The v2.1 protocol send the
  // message two time 
  sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));

  // Wait for 30 seconds before send a new message 
  SEND_LOW();

dht_error:
  Serial.print("wait...\n");
#ifdef DEBUG
  delay(10000);
  blinkLed();
#else
  for(int i=0; i<6; i++){
     delay(10000);
     Serial.print(".");
     blinkLed();
  }
#endif
}

void blinkLed()
{
  // blink the led
  digitalWrite(LED_PIN, HIGH);
  delay(30);   
  digitalWrite(LED_PIN, LOW);
}
