#include <SoftwareSerial.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

/*
 * PIN MAP
 * 2  ESPSerial TX
 * 3  ESPSerial RX
 * 10 DFPlayerSerial RX
 * 11 DFPlayerSerial TX
 * A3 DFPlayer Busy
 */
 
SoftwareSerial DFPlayerSerial(10, 11);  //RX, TX  
SoftwareSerial ESPSerial(2,3);  //RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int getEventNum();

void setup()
{
  //open Serial port & wait utill open
  Serial.begin(9600);
  while(!Serial);

  //open SoftwareSerial port & wait utill open
  DFPlayerSerial.begin(9600);
  while(!DFPlayerSerial);
  ESPSerial.begin(9600);
  while(!ESPSerial);

  //initialize DFPlayer
  DFPlayerSerial.listen();
  Serial.println();
  Serial.println(F("Initializing DFPlayer ... "));
  if(!myDFPlayer.begin(DFPlayerSerial)) 
  {
    Serial.println(F("Unable to begin DFPlayer. Please reset ... "));
    while(true);
  }
  Serial.println(F("DFPlayer online!!"));
  
  //setting initial values
  myDFPlayer.disableLoop(); 
  myDFPlayer.volume(15); //set volume(0~30)
  myDFPlayer.play(5); //play initialize check mp3 file
  delay(1000);
}

void loop()
{
  //read from serail message and play certain mp3 file.
  DFPlayerSerial.listen();
  int event_num  = getFromPort();

  if(event_num != 0)
  {
    myDFPlayer.play(event_num); 
    delay(1500);
    while(analogRead(A5) < 400);
    delay(50);
  }   
  //show detailed msg on Serial Monitor
  if(myDFPlayer.available())
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());  //print the detailed msg
  }
  
  //show esp AT command echo
  ESPSerial.listen();
  if (ESPSerial.available()) {
    Serial.write(ESPSerial.read());
  }
  if (Serial.available()) {
    ESPSerial.write(Serial.read());
  }
}

void printDetail(uint8_t type, int value)
{
  switch (type) 
  {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayer Error!"));
      switch (value)
      {
        case Busy:
          Serial.println(F("Card Not Found!"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping!"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack!"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match!"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out Of Bound!"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File!"));
          break;
        case Advertise:
          Serial.println(F("In Advertise!"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Get event number from serial message.
 * messages from Serial is like below
 * Event:0  | ideal state
 * Event:1  | event occured. play 0001.mp3
 */

int getFromPort()
{
  int event_num = 0;
  String str = Serial.readString(); // 
  int idx_colon = str.indexOf(":"); // index of ":"
  int idx_terminator = str.indexOf("\r\n");
  //아무 입력도 없을때 
  if((char)str[0] <= 31) 
  { 
    //do nothing 
  }
  //Event:로 시작할 때 
  else if(str.substring(0, idx_colon) == "Event")
  {
    //convert event number from string to int
    event_num = str.substring(idx_colon+1, idx_terminator).toInt();
    Serial.print("Event:");
    Serial.println(event_num);
  }
  else
  {
    Serial.println("Invalid Command!");
    event_num = -1;
  }
  return event_num;
}
