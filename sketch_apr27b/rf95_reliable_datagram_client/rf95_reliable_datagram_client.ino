// rf95_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with the RFM95W 

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <SoftwareSerial.h>


#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

//// GPS
//
SoftwareSerial mySerial(4,5);//(RX,TX), (4->GPS_TX,5->GPS_RX)
// Singleton instance of the radio driver
RH_RF95 driver;
//RH_RF95 driver(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);
uint8_t data[30]="Viva Raquelandia";
// Dont put this on the stack:
uint8_t buf[10];

void setup() 
{
  mySerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
}


void loop() {
  bool newData = false;
  int index = 0;
  char temp = 0;
  String dataString = "";
  while(mySerial.available())
  {
    temp = mySerial.read();
    //Serial.print(temp);
    dataString += String(temp);
    //Serial.println(index);
    index++;
    if(index>2000)
      break;
  }
  //Serial.println(dataString);
  String coordenadas = "";
//  // Process NMEA Sentence
  if (dataString.indexOf("GPGLL") > 0) {
    coordenadas = dataString.substring(dataString.indexOf("GPGLL")+6, dataString.indexOf("GPGLL") + 32);
  } else {
    coordenadas = dataString.substring(dataString.indexOf("GPRMC")+18, dataString.indexOf("GPRMC") + 44);
  }

 Serial.println("Las coordenadas son");
 Serial.println(coordenadas);

   
  String finalstring = coordenadas;
  Serial.println("Sending to rf95_reliable_datagram_server");
  finalstring.getBytes(data,sizeof(data));
  // Send a message to manager_server
  Serial.print("Send");
  //Serial.print(lux);
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      Serial.println(driver.lastSNR());
    }
    else
    {
      Serial.println("No reply, is rf95_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  driver.sleep(); // PONERLO EN BAJO CONSUMO
  delay(5000);
}


// Para probar con salida analógica y digital
//String sensorRawToPhys(int raw){
//// Conversion rule
//float Vout = float(raw) * (VIN / float(1023));// Conversion analog to voltage
//float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
//int phys=500/(RLDR/1000); // Conversion resitance to lumen
//return (String) phys;
//}
