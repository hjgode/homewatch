/*
 * TempHumidity.cpp
 *
 * Sample driver application to test the Dht11 module.
 */
#include "Arduino.h"
#include "Dht11.h"

enum {
    // The data I/O pin connected to the DHT11 sensor
    DHT_DATA_PIN = 2,

    // The baud rate of the serial interface
    SERIAL_BAUD  = 9600,

    // The delay between sensor polls.
    POLL_DELAY   = 2000,
};

/*
 * setup
 *
 * One-time initialization of the module.
 */
void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.print("Dht11 Lib version ");
    Serial.println(Dht11::VERSION);
}

/*
 * loop
 *
 * Code to be executed repeatedly.
 */
void loop() {
    static Dht11 sensor(DHT_DATA_PIN);

    switch (sensor.read()) {
    case Dht11::OK:
        Serial.print("Humidity (%): ");
        Serial.println(sensor.getHumidity());

        Serial.print("Temperature (C): ");
        Serial.println(sensor.getTemperature());
        break;

    case Dht11::ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;

    case Dht11::ERROR_TIMEOUT:
        Serial.println("Timeout error");
        break;

    default:
        Serial.println("Unknown error");
        break;
    }

    delay(POLL_DELAY);
}
