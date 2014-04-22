Introduction
------------

I always aware of relative humidity in our bath room. Recently we buyed an air dryer, but I was very dis-satisfied by the on-off cycles of this stupid device. Although I set it to 70 or 80% RH, it switches on for very short periods at times I did not see this high humidity. I am already driving a network of wireless sensors within the 433MHz band and fill a database with the readings of three sensors. I added one more using one Arduino Uno R3 with a DHT22 sensor and a 433MHz transmitter. So I could track the temperature and humidity values of the bath too.

In general controlling humidity can often not be done simply by watching for the current RH% and then switch on or off a dryer or fan. RH% will vary with temperature changes and depends on the outside humidity. In example you will not reach 50% relative humidity (RH) in a room, if the ouside has 75% RH and the room is not sealed.

I decided to control the air dryer from the arduino with some better logic then just going on/off at a specific humidity level.

Background
----------

When showring starts in the bath, the humidity rapidly grows. I would like the arduino to recognize this peak change and to switch the dryer on for a maximum time.

As the arduino was already equiped with the 433MHz transmitter and there are power switches that are also controlled by 433MHz and I would like to avoid any contact with high voltage, I decided to control the power from the same transmitter.

So, we have the arduino, a DHT22 (the DHT11 was too bad), a 433 MHz receiver. For some visual feedback I added a LCD, but it is not needed for the function.

Using the code
--------------

The code uses two buffers to store the recent humidity measurements. Only the last values are kept. Every time a new value is pushed, the oldest value is removed. I use one 'short-time' buffer, which is updated every minute. The second buffer is the long-time buffer (memory), which is filled every time when the buffer capacity of the first buffer is reached. So lets say I store 5 values at all in the first buffer, then the second buffer is updated on every 5th update.

If the newest value is above a treshold of the last one and the values are increasing, I set a state flag which represents the powering of the dryer. The second buffer is currently not used but would be usefull to switch the dryer off, when the humidity is below the normal long-time values.

I have implemented a base class (using code of circular buffer) and init two of these buffer classes in my new class.

ByteBuffer.h (a 'stack' like buffer):

```    #include <stdlib.h>
    #ifndef BYTEBUFFER_H
    #define BYTEBUFFER_H
    #define byte unsigned char
    class ByteBuffer
    {
    public:
        ByteBuffer();
        // This method initializes the datastore of the buffer to a certain sizem the buffer should NOT be used before this call is made
        void init(unsigned int buf_size);
        // This method resets the buffer into an original state (with no data)
        void clear();
        // This releases resources for this buffer, after this has been called the buffer should NOT be used
        void deAllocate();
        // Returns how much space is left in the buffer for more data
        int getSize();
        // Returns the maximum capacity of the buffer
        int getCapacity();
        // This method returns the byte that is located at index in the buffer but doesn't modify the buffer like the get methods (doesn't remove the retured byte from the buffer)
        byte peek(unsigned int index);
        //
        // Put methods, either a regular put in back or put in front
        //
        int putInFront(byte in);
        int put(byte in);
        int push(byte in);    //HGO
        byte getMedian();    //HGO
        byte getMax();    //HGO
        byte getMin();    //HGO
        byte getState();    //HGO
        void setTreshold(byte t);    //HGO
        byte getTreshold();    //HGO
        int getDirection();
        int getStateDuration();
        int putIntInFront(int in);
        int putInt(int in);
        int putLongInFront(long in);
        int putLong(long in);
        int putFloatInFront(float in);
        int putFloat(float in);
        //
        // Get methods, either a regular get from front or from back
        //
        byte get();
        byte getFromBack();
        int getInt();
        int getIntFromBack();
        long getLong();
        long getLongFromBack();
        float getFloat();
        float getFloatFromBack();
        void setMaxDuration(int);
    protected:
        byte* data;
        unsigned int capacity;
        unsigned int position;
        unsigned int length;
        byte state;
        byte treshold;
        int stateDuration;  //start if state=1 for
        int maxDuration;    //max duration (ie 5)
        int direction;
    };
    #endif // BYTEBUFFER_H 

This class is used two times for a long and short time storage in NewBuffer:

```    #ifndef NEWBUFFER_H
    #define NEWBUFFER_H
    #include "ByteBuffer.h"
    class newBuffer
    {
        public:
            newBuffer();
            ~newBuffer();
            void init(int s);
            ByteBuffer bytebuffer1;
            ByteBuffer bytebuffer2;
            byte peek1(unsigned int index);
            byte peek2(unsigned int index);
            int getSize1();
            int getSize2();
               byte getMedian1();    //HGO
            byte getMax1();    //HGO
            byte getMin1();    //HGO
            byte getState1();    //HGO
            byte getTreshold1();    //HGO
               byte getMedian2();    //HGO
            byte getMax2();    //HGO
            byte getMin2();    //HGO
            byte getState2();    //HGO
            void setTreshold(byte t);    //HGO
            byte getTreshold2();    //HGO
            int getDirection1();
            int getStateDuration1();
            int getDirection2();
            int getStateDuration2();
            void setMaxDuration1(int);
            void setMaxDuration2(int);
            int put(byte in);
            int push(byte);
        protected:
            int iCount;
            byte treshold;
            int capacity;
            int maxDuration;    //max duration (ie 5)
        private:
    };
    #endif // NEWBUFFER_H

You see there are getters to get values from the short a long time buffer.

The main work is done when a new value is pushed to the stack:

```    int newBuffer::push(byte in){
        //static byte lastByte;
        iCount++;
        bytebuffer1.push(in);
        if(iCount==capacity){
            bytebuffer2.push(in);
            iCount=0;
        }
        return 0;
    }  

You see that the second (long time) buffer get's a push only every x times.

The arduino sketch:

```    ...
    void setup(){
      Serial.begin(SERIAL_BAUD);
      Serial.println("ThermoHygroTransmitter version 4 (power save 2)");
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
            send_buffer.push((byte)lastHumi);
            state=send_buffer.getState1();
            //send switch command
            if(state==0)
              switchOnOff(false);
            else
              switchOnOff(true);              
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
          

Here is the push of the new measured RH value and the check of the state property to switch an outlet on/off:

```            send_buffer.push((byte)lastHumi);
        state=send_buffer.getState1();
            //send switch command
            if(state==0)
              switchOnOff(false);
            else
              switchOnOff(true);

When a new value is pushed, we do some calculations and start/continue the powering of a dryer fan using the state var:

```    int ByteBuffer::push(byte in){
    if(length < capacity){
            // save data byte at end of buffer
            data[(position+length) % capacity] = in;
            // increment the length
            length++;
        }
        else{
            //move all one down
            unsigned int i=0;
            for(i=0; i<length-1; i++)
                data[i]=data[i+1];
            data[length-1]=in;
        }
        //check if to switch FAN on and keep for nexxt 5 iteratrions
        if( ((getMax()-getMin())>treshold) && (getDirection()>0) ){
            //set state=1 only if stateDuration is less then maxDuration
            if(stateDuration<maxDuration){
                state=1;
            }
        }
        if(state==1 || stateDuration>0)
            stateDuration++;    //increment stateDuration
        if(stateDuration>maxDuration){
            if(state==1){
                state=0;
            }
            stateDuration=0;
        }
         return 0;
    }
     

The above code first pushes the new value onto the stack. If the stack is full in regards of the number of maximum values, the oldest value is removed.

Then we check if max and min of all values is greater than a treshold and if the value shows an increase. If so, we start to set the state var. The state var is used to control the powering of a dryer fan and will remain set for maxDuration push cycles. So we do not switch on/off the fan directly pending to the values but implement a follow-up time. As we push a new value every minute and maxDusration is 9, we have a follow-up time of 9 minutes.

As I log (send) all values to a php/mysql web server, I can control what the sketch does:

![](http://hjgode.de/homewatch/phpplot2_7march.png)As you see, the state powered the outlet three times, every time the treshold of the humidity increase was hit. We are three people showering at the morning ;-)

Acknowledements:
----------------

ByteBuffer is based on [Circular Buffer](http://siggiorn.com/?p=460 "Circular Buffer")

[fuzzylogic 433MHz for arduino](https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home "fuzzylogic 433MHz for arduino") for his SensorTransmitter code

[rc-switch code](http://code.google.com/p/rc-switch/) to control the 433MHz wireless power outlet

ToDo:
-----

Cleanup unused code from ByteBuffer and newBuffer
