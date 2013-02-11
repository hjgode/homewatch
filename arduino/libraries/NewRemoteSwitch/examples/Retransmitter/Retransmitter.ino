/*
* Demo for RF remote switch receiver.
 * For details, see NewRemoteReceiver.h!
 *
 * Connect the transmitter to digital pin 11, and the receiver to digital pin 2.
 *
 * When run, this sketch waits for a valid code from a new-style the receiver, 
 * decodes it, and retransmits it after 5 seconds.
 */

#include <NewRemoteReceiver.h>
#include <NewRemoteTransmitter.h>

void setup() {
  // See example ShowReceivedCode for info on this
  NewRemoteReceiver::init(0, 2, retransmitter);
}

void loop() {
}

void retransmitter(NewRemoteCode receivedCode) {  
  // Disable the receiver; otherwise it might pick up the retransmit as well.
  NewRemoteReceiver::disable();

  // Need interrupts for delay()
  interrupts();

  // Wait 5 seconds before sending.
  delay(5000);

  // Create a new transmitter with the received address and period, use digital pin 11 as output pin

  NewRemoteTransmitter transmitter(receivedCode.address, 11, receivedCode.period);

  // Switch type 0 = switch off, type 1 = switch on, type 2 = set dim level.
  if (receivedCode.switchType == 2) {
    // Dimmer signal received
    transmitter.sendDim(receivedCode.unit, receivedCode.dimLevel);	
  } 
  else {
    // On/Off signal received

    if (receivedCode.groupBit) {
      // Send to the group
      transmitter.sendGroup(receivedCode.switchType);
    } 
    else {
      // Send to a single unit
      transmitter.sendUnit(receivedCode.unit, receivedCode.switchType);
    }
  }

  NewRemoteReceiver::enable();
}

