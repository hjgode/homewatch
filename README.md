homewatch
=========

Using atmega AVR (Arduino UNO, Pollin NETIO) to log Energy Cost 3000 and TFA Dostmann temerature and humidity wireless messages    

I have started to create a system to capture temperature and humidity messages send wireless by TFA Dostmann sensors at 433MHz using an Arduino UNO and a simple 433MHz receiver modul. The similar code runs on a Pollin AVR-Netio.

I also have working code to receive the Voltcraft Engergy Cost 3000 wireless messages using an Arduino UNO and a RFM12 868MHz transceiver modul.

The data should be posted by the Arduino or AVR-NetIO to a PHP/MySQL web server that currently runs on a modded Bufallo Linkstation LS-Pro.

The final goal is to log all data on the PHP/MySQL webserver and give various screens with graphs to show the logged data.


All code is free for use.
