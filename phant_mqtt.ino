
#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>
#include "Credentials.h"

// WiFly Library that has the same interface as the Arduino Ethernet client
// https://github.com/dpslwk/WiFly
WiFlyClient wifi;
// MQTT library for the Arduino Ethernet Shield
// https://github.com/knolleary/pubsubclient
PubSubClient client("data.sparkfun.com", 1883, callback, wifi);

// comment out the following line to disable debugging
#define DEBUG

// debugging macros
#ifdef DEBUG
  #define DEBUG(msg) Serial.println(msg)
#else
  #define DEBUG(msg)
#endif

// topics
char button[] = "output/WGRd11VpGLT5mr00OrQ7/button";
char sliderOne[] = "output/WGRd11VpGLT5mr00OrQ7/slider_one";
char sliderTwo[] = "output/WGRd11VpGLT5mr00OrQ7/slider_two";

// pins
int buttonPin = 4;
int sliderOnePin = 5;
int sliderTwoPin = 6;

/**
 * setup
 *
 * decalled when the sketch starts. this
 * is where the network connection is initialized,
 * and where the connection to the MQTT broker is started.
 */
void setup() {

  Serial.begin(9600);

  // wifly module init
  WiFly.begin();
  
  // set pins to output
  pinMode(buttonPin, OUTPUT);     
  pinMode(sliderOnePin, OUTPUT);     
  pinMode(sliderTwoPin, OUTPUT);     

  // start sketch with all LEDs off
  buttonState(buttonPin, LOW);
  sliderState(sliderOnePin, 0);
  sliderState(sliderTwoPin, 0);

  // attempt to conenct to access point
  if (!WiFly.join(ssid, passphrase)) {
    DEBUG("Association failed");
    while (1) {} // hang if connection failed
  }

  DEBUG("connected to the network");
  DEBUG("attempting connection to the mqtt server...");

  if(client.connect("arduinomqtt")) {

    DEBUG("mqtt client connected & subscribed");

    // all of these will be handled by the callback function
    client.subscribe(button);
    client.subscribe(sliderOne);
    client.subscribe(sliderTwo);

  } else {

    DEBUG("mqtt connection failed.");
    while(1) {} // hang if connection to mqtt failed

  }

}

/**
 * loop
 *
 * a sisyphean function that allows
 * the mqtt client to stay connected &
 * process incoming messages
 */
void loop() {
  client.loop();
}

/**
 * callback
 *
 * called when a message is received that maches
 * a topic that was subscribed to in setup()
 */
void callback(char* topic, byte* payload, unsigned int length) {

  String t = String(topic);
  int value = toInt(payload, length);

  if(t.equals(button)) {

    DEBUG("button topic match");
    buttonState(buttonPin, value);

  } else if(t.equals(sliderOne)) {

    DEBUG("slider one topic match");
    sliderState(sliderOnePin, value);

  } else if(t.equals(sliderTwo)) {

    DEBUG("slider two topic match");
    sliderState(sliderTwoPin, value);

  }

  DEBUG(t);
  DEBUG(value);

}

/**
 * toInt
 *
 * converts payload to an int
 */
int toInt(byte* payload, int length) {

  int i;
  char val[10];

  for(i = 0; i < length; i++) {
    val[i] = payload[i];
  }

  val[i] = '\0';

  return atoi(val);

}

/**
 * buttonState
 *
 * changes the state of the digital pin
 * to high or low based on payload value
 */
void buttonState(int pin, int on) {

  if(on != 1 && on != 0) {
    DEBUG("INVALID BUTTON PAYLOAD VALUE");
    return;
  }

  if(on == 1) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }

}

/**
 * sliderState
 *
 * changes the state of the digital pin
 * to high or low based on payload value
 */
void sliderState(int pin, int value) {

  if(value < 0 || value > 255) {
    DEBUG("INVALID SLIDER PAYLOAD VALUE");
    return;
  }

  analogWrite(pin, value);

}



