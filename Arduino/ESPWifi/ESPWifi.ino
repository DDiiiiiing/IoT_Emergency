#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID      [YOUR_SSID]
#define PASSWORD  [YOUR_PW]
#define HOST_NAME [YOUR_HOST_IP]
#define HOST_PORT 4000

SoftwareSerial ESPSerial(8,9);
ESP8266 wifi(ESPSerial);

void printUsage()
{
  uint8_t buf[] = "Usage\nEvent:1 <= play 1st\n2 <= play 2nd";
  wifi.send(buf, strlen(buf));
}

bool isConn = false;

void setup()
{
  Serial.begin(9600);
  Serial.print("Setup begin!\r\n");
  wifi.restart();
  delay(2000);
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

  Serial.print("Setup End!\r\n");
}

void loop()
{
  if(isConn == false)
  {
    while(true)
    {
      if(wifi.createTCP(HOST_NAME, HOST_PORT))
      {
        Serial.print("Create TCP ok\r\n");
        isConn = true;
        printUsage();
        break;
      }
      else
      {
        Serial.print("Create TCP err\r\n");
      }
    }
  }

  uint8_t buffer[128] = {0};

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if(len>0)
  {
    Serial.print("Received:[");
    for(uint32_t i = 0; i < len; i++)
    {
      Serial.print((char)buffer[i]);
    }
    Serial.print("]\r\n");

    char command = buffer[0];

    switch (command)
    {
      case '1':
        Serial.println("case:1");
        break;
      case '2':
        Serial.println("case:2");
        break;
      default:
        Serial.println("IDK");
    }
  }
}
