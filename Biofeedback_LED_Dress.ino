/* Pulse Sensor Amped with NeoPixels
Pulse an arbitrary number of Adafruit NeoPixels based on a heartbeat sensor
The pulsesensor.com code needs to be in module interrupt.ino in the sketch directory
   http://pulsesensor.com/pages/pulse-sensor-amped-arduino-v1dot1
Code also uses the Adafruit NeoPixel and Pixie library codes discussed at
   https://learn.adafruit.com/adafruit-neopixel-uberguide
Version 1.0 by Mike Barela for Adafruit Industries, Fall 2015 
Slight modifications by jeffyactive for GenieMob, 2017
Additional modifications by leahrebecca, 2021
*/
#include <Adafruit_NeoPixel.h>    // Library containing 

#include "SoftwareSerial.h"
#include "Adafruit_Pixie.h"



// Behavior setting variables
int pulsePin = 11;                // Pulse Sensor purple wire connected to analog pin X
int blinkPin = 13;                // Digital pin to blink led at each beat
int fadePin  = 8;                 // pin for internal neopixel (heartbeat)
int stripPin = 6;                 // pin for external neopixel strip
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int redLedPin = 7;

SoftwareSerial pixieSerial(-1, stripPin);

// these variables are volatile because they are used during the interrupt service routine
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Set up use of NeoPixels
const int NUMPIXELS = 2;            // Put the number of NeoPixels you are using here
const int BRIGHTNESS = 255;          // Set brightness of NeoPixels here
Adafruit_NeoPixel instrip = Adafruit_NeoPixel(1, fadePin, NEO_GRB + NEO_KHZ800);
Adafruit_Pixie exstrip = Adafruit_Pixie(NUMPIXELS, &pixieSerial);

void setup(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(redLedPin, OUTPUT);
  instrip.begin();
  Serial.begin(9600);
  Serial.println("Ready to Pixie!");

  pixieSerial.begin(115200); // Pixie REQUIRES this baud rate
 
  instrip.setBrightness(BRIGHTNESS);
  exstrip.setBrightness(BRIGHTNESS);
  instrip.setPixelColor(0, instrip.Color(0, 0, 0));
  for (int x=0; x < NUMPIXELS; x++) {  // Initialize all pixels to 'off'
     exstrip.setPixelColor(x, exstrip.Color(0, 0, 0));
  }
  instrip.show();                   // Ensure the pixels are off 
  exstrip.show();                   // Ensure the pixels are off 
  delay(1000);                      // Wait a second
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
}

void loop(){
  if (QS == true){                    // Quantified Self flag is true when arduino finds a heartbeat
     fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
     QS = false;                      // reset the Quantified Self flag for next time 
  }
  ledFadeToBeat();                    // Routine that fades color intensity to the beat
  setExStrip();
  delay(20);                          //  take a break
}

void ledFadeToBeat() {
    fadeRate -= 15;                         // Set LED fade value
    fadeRate = constrain(fadeRate,0,255);   // Keep LED fade value from going into negative numbers
    setInStrip(fadeRate);                     // Write the value to the NeoPixels 
}

void setInStrip(int r) {   // Set the strip to one color intensity (red)
   int g = 0;              // Green is set to zero (for non-red colors, change this)
   int b = 0;              // Blue is set to zero (for non-red colors, change this)
   instrip.setPixelColor(0, instrip.Color(r, g, b));
   instrip.show();
}

void setExStrip() {
  int g = 300 - BPM;
  int r = (BPM - 60) * 2;
  if(g > 255) {
    g = 255;
  }
  if(r > 255) {
    r = 255;
  }
  exstrip.setPixelColor(0, exstrip.Color(r, g, 0));
  exstrip.show();
}
