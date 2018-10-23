/*
  Web client

 This sketch connects to a website through a GSM shield. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it 
 to the Serial monitor.

 Circuit:
 * GSM shield attached to an Arduino
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMExamplesWebClient

 */

// libraries
#include <MKRGSM.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "orbcomm01.com.attz" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess; 

// URL, path & port (for example: arduino.cc)
char server[] = "ws.uscubed.com";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    //client.print("HAHAHAHAAHA");
    //client.write(32);
    // Make a HTTP request:

    Serial.println("Connected to server");
    // Make a HTTP request
    String content = "Hey";
  client.println("POST /Arduino.aspx HTTP/1.1");
  client.println("Host: ws.uscubed.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(content.length());
  client.println();
  client.println(content);
  } 
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
