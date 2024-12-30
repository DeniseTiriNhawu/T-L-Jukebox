#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
SoftwareSerial softSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
//light sensor
int lightSensorPin = A0;  // Photoresistor connected to analog pin A0
int lightThreshold = 600; // the threshold for the light
bool isPlaying1 = false;  // boolean to check is song 1 is playing
bool isPlaying2 = false; //boolean to check if song 2 is playing
bool isPlaying3 = false; //boolean to check if song 3 is playing
//RGB LEDs
int red = 8; //pin for red rgb
int blue = 12; //pin for blue
int green = 9; //pin for green
int red2 = 3; // pin red for second led
int blue2 = 13; // pin for blue second led
int green2 = 4; // pin for green second led
int step = 1; //step used for seqeunces
//temperature sensor
int temperatureSensorPin = A1; // pin for temperature sensor
int degreesCelsius; // value read for sensor in degress
int startTemp; // temp once the code has run
int changeInTemp = 0; // the difference between startTemp and currentTemp
//microphone sensor
int digitalmicrophoneSensorPin = 2; // digital pin for microphone sensor
int analogmicrophoneSensorPin = A2; // analog pin for microphone sensor
int digitalMicrophone;
int analogMicrophone;
int state = 1;
//turn off button
int buttonPin = 5; // pin for push button
int val;
int prevVal;
bool lightOn = false;  // on, or not?
void setup() {
 Serial.begin(115200);
 softSerial.begin(9600);
  if (!myDFPlayer.begin(softSerial, /*isACK = */ false, /*doReset = */ false)) {  // Use serial to communicate with mp3.
   Serial.println("Error starting DFplayer");
 }
 myDFPlayer.volume(20);  // Set volume to a medium level (0 to 30)
  pinMode(red, OUTPUT); // outpu
 pinMode(green, OUTPUT);
 pinMode(blue, OUTPUT);
 pinMode(red2, OUTPUT);
 pinMode(green2, OUTPUT);
 pinMode(blue2, OUTPUT);
 pinMode(5, INPUT); // button input
}
void loop() {
 if (state == 1) {
   state1();
 } else if (state == 2) {
   state2();
 } else if (state == 3) {
   state3();
 }
}
void state1() { //function to switch on the jukebox. if the distance < 50, the jukebox is "off" and will not play music even if sensors are activated
 int echoPin = 7;
 int triggerPin = 6;
 bool on = false; //bool used to detect if on or not
 long distance1;
 while (!on) {
   pinMode(echoPin, INPUT);
   pinMode(triggerPin, OUTPUT);
   digitalWrite(triggerPin, LOW);
   delayMicroseconds(2);
   digitalWrite(triggerPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(triggerPin, LOW);
   long duration = pulseIn(echoPin, HIGH, 17400);
   //print out the value you read :
   distance1 = duration / 29 / 2;
   Serial.println(distance1);
   if (distance1 > 50) { //checks if distance is > 50, if so then do the following
     myDFPlayer.play(4); // our audio, explaining how to use the jukebox
     state = 2; //goes to state 2
     on = true;
   }
 }
}
void state2() { //function used to play the songs on command
 bool run = false; //bool used to detect if anything is playing yet or running
 while (!run) {
   int sensorReading = analogRead(lightSensorPin);  // Read the photoresistor value
   int thisPitch = map(sensorReading, 30, 1000, 120, 1500); // mapped the photoresistor values for better reading
   Serial.println(thisPitch);  // Print the reading to the Serial Monitor for debugging
   degreesCelsius = analogRead(A1) * (5000 / 1024.0) / 10;//read the input on analog pin 1 and convert to degrees celsius
   Serial.println(degreesCelsius); //Print the reading to the Serial Monitor for debugging
   temperatureSensorPin = analogRead(A1); // read the temperature sensor value
 
   delay(100);//delay for stability
   int analogmicrophoneSensorPin = analogRead(A2);  //read the microphone sensor value
   Serial.println(analogmicrophoneSensorPin); // print the reading to serial mointer for debugging
   if (thisPitch > lightThreshold && !isPlaying1) {  // If light is shining (above threshold) and song1 is not playing, do the following
     myDFPlayer.play(3);                             // Play "Flashing Lights by Kanye west"
     isPlaying1 = true;                             //song one is playing
     isPlaying2 = false;                            //song two is not playing
     isPlaying3 = false;                         // song three is not playing
    
   } else if (degreesCelsius > 25 && !isPlaying2) {  // if temp os above 25 and song two is not playing, do the following
     myDFPlayer.play(2);                             // Play "Fire ball by Pitbull"
     isPlaying2 = true; //song two is playing
     isPlaying1 = false; // song one is not playing
     isPlaying3 = false; //song three is not playing
    
   } else if (analogmicrophoneSensorPin > 180 && !isPlaying3) { // if microphone value > 180 and song three is not playing do the following:
     myDFPlayer.play(1);  // Play "Glimpse of Us by Joji"
     isPlaying3 = true; //song three is  playing
     isPlaying1 = false; // song one is not playing
     isPlaying2 = false; //song two is not playing
    
   }
   if (isPlaying1) { //  if song one is playing , do the following light sequence
     setColor(255, 0, 0);  // Red Color
     delay(250);
     setColor(0, 255, 0);  // Green Color
     delay(250);
     setColor(0, 0, 255);  // Blue Color
     delay(250);
     setColor(255, 255, 255);  // White Color
     delay(250);
     setColor(170, 0, 255);  // Purple Color
     delay(250);
   } else if (isPlaying3) { //if song three is playing, do the following light sequence
     setColor(0, 0, 255);  // Blue Color
     delay(500);
     setColor(255, 255, 255);  // White Color
     delay(500);
   } else if (isPlaying2) { //if song two is playing, do the following light sequence
     setColor(255, 0, 0);  // Red Color
     delay(250);
     setColor(0, 255, 0);  // Green Color
     delay(250);
   }
   val = digitalRead(5); // digital pin for STOP button
   if (prevVal == LOW && val == HIGH) { // if button is pressed, switch off the leds and go to state 3
     prevVal = val;
     run = true;
     state = 3;
     delay(10);
     digitalWrite(red, LOW);
     digitalWrite(green, LOW);
     digitalWrite(blue, LOW);
     digitalWrite(red2, LOW);
     digitalWrite(green2, LOW);
     digitalWrite(blue2, LOW);
   }
 }
}
void setColor(int redValue, int greenValue, int blueValue) { // function used to output the different colours to the rgb leds
 analogWrite(red, redValue);
 analogWrite(red2, redValue);
 analogWrite(green, greenValue);
 analogWrite(green2, greenValue);
 analogWrite(blue, blueValue);
 analogWrite(blue2, blueValue);
}
void state3() { // function used to stop the music
 myDFPlayer.stop();
}
