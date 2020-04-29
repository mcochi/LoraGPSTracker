/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h> 
#include <ArduinoJson.h>
#include "secrets.h"

const char* ssid     = STASSID;
const char* password = STAPSK;
static String timetoarrive;
static String timetoarriveci;
int selectbus = 39;
uint8_t buf[10];


//const char* host = "djxmmx.net";
//const uint16_t port = 17;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(9600);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  //Serial.println();
  //Serial.println();
  //Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    //Serial.print(".");
    delay(500);
  }

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {
  // Recogemos lo que viene de la transmisión LORA
  String payloadfromuno;
  String lumen;
  String presionado;

  while(Serial.available() > 0) {
    //Serial.println("Comienzo a leer");
    char c = Serial.read();
    payloadfromuno += c;
  }
  Serial.println(payloadfromuno);
  lumen = payloadfromuno.substring(0,payloadfromuno.indexOf(';'));
  presionado = payloadfromuno.substring(payloadfromuno.indexOf(';')+1,payloadfromuno.length()-1);
  Serial.println(lumen);
  Serial.println(presionado); 
  String body = "{\"lumen\":" + lumen + ",\"presion\":" + presionado +"}";
  Serial.println(body);
 
  // Una vez obtenidos los datos habrá que realizar el POST
  WiFiClient client;
  String server = "192.168.1.36";
  // This will send the request to the server
  if (client.connect(server,9090)) {
    Serial.println("Conectado");
    client.print("POST /lora_poc");
    client.println(" HTTP/1.0");
    client.println("Host: 192.168.1.36");
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println();
    client.print(body);
  } else {
    Serial.println("Connection Failed!!!");
  }
  while (client.available() == 0) {
    //Espero respuesta del servidor
    delay(1000);
  }
  
  String payload ="";
  if (client.available() > 0) {
    //Serial.println("Respuesta del servidor");
      char c;
      while (client.available() > 0) {
        c = client.read();
        payload += c;
    } 
  }

  Serial.println(payload);

  client.stop();

  //Serial.println("wait 5 sec...");
  delay(4000);
}
