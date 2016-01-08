// compiler error handling
// we only support Arduino 1.6.6 or greater
#if ARDUINO < 10606
  #error Please upgrade your Arduino IDE to 1.6.6 or greater
#else 
  // check that Bare Conductive Touch Board is selected in Tools -> Board
  #if !defined(ARDUINO_AVR_BARETOUCH) || defined(IPAD_COMPAT)
    #error Please select "Bare Conductive Touch Board" in the Tools -> Board menu.
  #endif
#endif

#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11); // RX | TX
// Connect the HC-06 TX to the Arduino RX on pin 10. 
// Connect the HC-06 RX to the Arduino TX on pin 11 through a voltage divider.
// Arduino Duemilanove works on BTserial(2, 3)

int ledPin = 13;      // use the built in LED on pin 13 of the Uno
char messageCode = ' ';
int flag = 0;         // make sure that you return the state only once

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;

// sd card instantiation
SdFat sd;
 
void setup() {
  Serial.begin(57600);

  // uncomment this line when using the serial monitor
  //while (!Serial) ; {} 
  
  Serial.println("Starting...");
    
  // HC-06 default serial speed is 9600
  BTserial.begin(9600);
  
  pinMode(ledPin, OUTPUT);

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
 
  result = MP3player.begin();
  MP3player.setVolume(0,0);
  
  if (result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
  }
}
 
void loop() {
  // Keep reading from HC-06 and send to Arduino Serial Monitor
  messageCode = ' ';
  if (BTserial.available()) {  
    messageCode = BTserial.read();
    flag = 0;
    Serial.print("serial sent: ");
    Serial.println(messageCode);
  }

  if (messageCode == '0') {
    // messageCode 0 to stop
    digitalWrite(ledPin, LOW);
    if (flag == 0) {
      Serial.println("LED: off");
      flag = 1;
    }

    if (MP3player.isPlaying()){
      MP3player.stopTrack();
    }
  } else if (messageCode != ' ') {
    digitalWrite(ledPin, HIGH);
    if (flag == 0) {
      Serial.println("LED: on");
      flag = 1;
    }

    int track = messageCode - '0';
    MP3player.playTrack(track);
    Serial.print("playing track ");
    Serial.println(track);
  }
    
  // Keep reading from Arduino Serial Monitor and send to HC-06
  if (Serial.available()) {
    BTserial.write(Serial.read());
  }
}

void flashLED() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}