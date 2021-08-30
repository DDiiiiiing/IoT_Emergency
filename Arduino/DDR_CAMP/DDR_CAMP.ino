#include <SoftwareSerial.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "ESP8266.h"

#define SSID  "12345678"
#define PASSWORD  "00000000"
#define HOST_NAME "192.168.228.169"
#define HOST_PORT 4000

enum{RED, GREEN, BLUE, OFF};
#define ledR  5
#define ledG  6
#define ledB  7

/*
 * PIN MAP
 * 5  led R
 * 6  led G
 * 7  led B
 * 8  ESPSerial RX
 * 9  ESPSerial TX
 * 10 DFPlayerSerial RX
 * 11 DFPlayerSerial TX
 * A4 DFPlayer Busy
 */
 /*
  * SOUND LIST
  * 0001    BeapBeap  ->  play(2)
  * 0002    Beap      ->  play(3)
  * 0003    팡파레     ->  play(4)
  * 0004    wow       ->  play(5)
  * 0005    online    ->  play(1)
  */
  
SoftwareSerial DFPlayerSerial(10, 11);  //RX, TX  
SoftwareSerial ESPSerial(8,9);

ESP8266 wifi(ESPSerial);
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
  //led pin setup
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  
  //open Serial port & wait utill open
  Serial.begin(9600);
  while(!Serial);

  //open SoftwareSerial port & wait utill open
  DFPlayerSerial.begin(9600);
  while(!DFPlayerSerial);
  ESPSerial.begin(9600);
  while(!ESPSerial);

  //============================================
  //        initialize DFPlayer
  //============================================
  DFPlayerSerial.listen();
  Serial.println(F("Initializing DFPlayer ... "));
  if(!myDFPlayer.begin(DFPlayerSerial)) 
  {
    Serial.println(F("Unable to begin DFPlayer. Please reset ... "));
    while(true);
  }
  //setting initial values
  myDFPlayer.disableLoop(); 
  myDFPlayer.volume(20); //set volume(0~30)
  Serial.println(F("DFPlayer online!!"));
  delay(500);
  myDFPlayer.play(1); //play initialize check mp3 file
  
  //============================================
  //        initialize ESP
  //============================================
  ESPSerial.listen();
  Serial.println(F("Initializing ESP Module ... "));
  wifi.restart();
  delay(1000);
  Serial.print("FW Version : ");
  Serial.println(wifi.getVersion().c_str());
  
  if(wifi.setOprToStationSoftAP())
  {  
    Serial.print("to station + softap ok\r\n");
  }
  else  
  {
    Serial.print("to station + softap err\r\n");
  }
  
  if(wifi.joinAP(SSID, PASSWORD))
  {
    Serial.print("Join AP success\r\n");
    Serial.print("IP : ");
    Serial.println(wifi.getLocalIP().c_str());
  }
  else
  {
    Serial.print("Join AP failure\r\n");
  }

  if(wifi.disableMUX())   
  {
    Serial.print("single ok\r\n");
  }
  else
  {
    Serial.print("single err\r\n");
  }

  Serial.println("ESP Module online!!");

}


unsigned long lasting;  //이벤트가 지속된 시간.
  
void loop()
{  
  chkESP();
  //==================================================
  //get String from Serial and TCP.
  //==================================================
  ESPSerial.listen();
  String tcp_str = getFromESP();
  String serial_str = Serial.readString(); // 

  //when receive Q or q, program end
  Serial.println(serial_str);
  if(serial_str[0] == 'q' || serial_str[0] == 'Q')
  {
    //input [Quit] on serial
    char *quit = "q";
    wifi.send((const uint8_t*)quit, strlen(quit));
    Serial.println("manual quit ... ");
    while(1); //end
  }
  
  int event_num_tcp = EventNum(tcp_str);
  int event_num_serial = EventNum(serial_str);
//  //led
//  setLed(event_num_tcp==0?OFF:RED);
//  setLed(event_num_serial==0?OFF:RED);
  
  //==================================================
  //read from serial message and play certain mp3 file.
  //==================================================
  /*
   * Event1, 2 ..등 특이사항이 발생하고 같은 신호가 일정시간(여기서는 7초) 
   * 이상 같은 신호가 들어오면 상황이 발생했다고 인식함. 
   * 상황 발생을 인식히면 일정 시간(여기서는 10초)동안 경고음+LED 알림
   */   
  DFPlayerSerial.listen();
  Serial.println(lasting);
  if(event_num_tcp == 0 || event_num_tcp == -1 )  // 0: 아무 일 없음. 
  {                                               //-1: 에러
    setLed(BLUE);
    lasting = millis(); //중간에 이벤트가 아닌 상황발생시 갱신
  }
  else{ //이벤트가 발생한 상황
    if(millis() - lasting > 7000) //대략 7초이상 지났을때 
    {
      bool isSend = false;
      while(millis() - lasting < 17000) //추가로 10초이내동안 소리+led 알림
      {
        if(event_num_tcp == 1)  //1:이벤트1 발생.
        {
          setLed(RED);
          myDFPlayer.play(2); 
          while(analogRead(A4) < 400);  //wait until not busy
          if(!isSend)
          {
            //서버에 이벤트 알리기
            ESPSerial.listen();
            char *event = "E1";
            wifi.send((const uint8_t*)event, strlen(event));
            Serial.println("ENQ");
            isSend = true;
            DFPlayerSerial.listen();
          }
        }
        if(event_num_tcp == 2)  //2:이벤트2 발생.
        {
          setLed(GREEN);
          if(!isSend)
          {
            //서버에 이벤트 알리기
            ESPSerial.listen();
            char *event = "E2";
            wifi.send((const uint8_t*)event, strlen(event));
            Serial.println("ENQ");
            isSend = true;
            DFPlayerSerial.listen();
          }
          myDFPlayer.play(3); 
          while(analogRead(A4) < 400);  //wait until not busy
        }
      }
    }
  }
  
//  //test script
//  if(event_num_tcp != 0)
//  {
//    myDFPlayer.play(event_num_tcp); 
//    delay(1500);
//    while(analogRead(A4) < 400);  //wait until not busy
//    delay(50);
//  }   
//  //for test
//  if(event_num_serial != 0)
//  {
//    myDFPlayer.play(event_num_serial); 
//    delay(1500);
//    while(analogRead(A4) < 400);  //wait until not busy
//    delay(50);
//  }   
//  

  //show detailed message on Serial Monitor
  if(myDFPlayer.available())
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());  //print the detailed msg
  }
}

/* 
 * Get event number.
 * messages from Serial is like below
 * Event:0  | ideal state
 * Event:1  | event occured. play 0001.mp3
 */
int EventNum(String str)
{
  int event_num = 0;
  int idx_colon = str.indexOf(":"); // index of ":"
  int idx_terminator = str.indexOf("\r\n");
  //아무 입력도 없을때 
  if((char)str[0] <= 31) //command code from ASCII
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

/*
 * Check TCP connect everytime
 * save from TCP buffer and print to Serial
 */
 
bool isConn = false;
void chkESP()
{ 
  if(isConn == false)
  {
    while(true)
    {
      if(wifi.createTCP(HOST_NAME, HOST_PORT))
      {
        Serial.print("Create TCP ok\r\n");
        isConn = true;
        break;
      }
      else
      {
        Serial.print("Create TCP err\r\n");
      }
    }
  } 
}

//get data from TCP and print to Serial and return int value
String getFromESP()
{
  uint8_t buffer[128] = {0};  //get from TCP
  String str = "";        //make buffer into String

  //ask server 
  char *enq = "ENQ";
  wifi.send((const uint8_t*)enq, strlen(enq));
  Serial.println("ENQ");

  //get from server
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if(len>0)
  {
    Serial.print("Received:");
    for(uint32_t i = 0; i < len; i++)
    {
      char buff = (char)buffer[i];
      Serial.print(buff);
      str.concat(buff);
    }
    Serial.print("\r\n");
    str.concat("\0");
  }

  return str;
}
//LED active when LOW
void setLed(int Color)
{
  switch(Color)
  {
    case RED:
      digitalWrite(ledR, HIGH);
      digitalWrite(ledG, LOW);
      digitalWrite(ledB, LOW);
      break;
      
    case GREEN:
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, HIGH);
      digitalWrite(ledB, LOW);
      break;
      
    case BLUE:
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, LOW);
      digitalWrite(ledB, HIGH);
      break;
      
    case OFF:
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, LOW);
      digitalWrite(ledB, LOW);
      break;
      
    default:
      break;
  }
}

//VERVOSE : print DFPlayer info
void printDetail(uint8_t type, int value)
{
  switch (type) 
  {
    case TimeOut:
      //Serial.println(F("Time Out!"));
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
