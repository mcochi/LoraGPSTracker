/*
  Demo code for GPS Shield
  It records the GPS information onto the TF card, 
  and display on serial monitor as well.
  
  http://makerstudio.cc
*/
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
const int chipSelect = 10;
SoftwareSerial mySerial(6,7);//(RX,TX), (6->GPS_TX,7->GPS_RX)
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {  
    return;
  } 
}

void loop()
{
  // make a string for assembling the data to log:
  char index = 0;
  char temp = 0;
  String dataString = ""; 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if(dataFile)
  {
  while(mySerial.available())
  {
    temp = mySerial.read();
    Serial.print(temp);
    dataString += String(temp);
    index++;
    if(index>200)
      break;
  }
  dataFile.print(dataString);
  
  dataFile.close();
  }else
  {
  Serial.println("Open file failed");
  }
} 
