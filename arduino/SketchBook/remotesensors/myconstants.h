//myconstants.h

#ifndef _MYCONSTANTS_H_
#define _MYCONSTANTS_H_

  // see     
  // ether.browseUrl(PSTR("/index.php"), "?channel=2&temp=111&humidity=55&millis=%lu", website, my_result_cb);

#define BAUDRATE 57600

  char webString[] PROGMEM = "?channel=%i&temp=%i&humidity=%i&millis=%lu";

  // ethernet interface mac address, must be unique on the LAN
  static byte mymac[] = { 0x00,0x22,0xf9,0x01,0xb2,0x28 };
  static byte myip[] = { 192,168,0,13 };
  static byte mymask[] = { 255,255,255,0 };
  static byte mygw[]   = { 192,168,0,250 };

  // remote website name
  char remoteWebsite[] PROGMEM = "LS-GLBB3"; //"bufallo";
  static byte remoteWebsiteIP[] = { 192,168,0,40 };
  static byte remotePort = 8081;
  
#endif
