//myweb.h

#ifndef _MYWEB_H_
#define _MYWEB_H_

#include <Ethernet.h>

#include "myconstants.h"

void pushUpdate(){
  Serial.println("pushUpdate()...");
  // push the current values in sensorData to the server
  ether.packetLoop(ether.packetReceive());
  // possibly look at http://blog.cuyahoga.co.uk/2012/05/theres-something-wrong-with-my-stash/
  // but this update is only called every 10 minutes
  //if (millis() > timer + REQUEST_RATE) {
  //  timer = millis();
    Serial.println("\n>>> REQ");
    // ensorData[i-1].temp / 10, sensorData[i-1].temp % 10, sensorData[i-1].humidity, sensorData[i-1].strDay, sensorData[i-1].strClock
    //ether.browseUrl(PSTR("/foo/"), "bar", website, my_result_cb);
    ether.browseUrl(PSTR("/index.php"), "?channel=2&temp=111&humidity=55", website, my_result_cb);
  //}
}

#endif
