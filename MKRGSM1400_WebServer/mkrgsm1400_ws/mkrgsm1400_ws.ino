/*
MKR GSM 1400

This sketch, for the MKR GSM 1400 board,
- Initialize the GSM module
- It will check every 1 hours, the ultrasonic & float sensor.
- Activate the relay for 7 seconds and take the ultrasonic sensor measurement.
- It will send message after taking the data from the sensor.
- Turn off the GSM module
- Sleep for 1 hours wait for the next cycle

last edited 10/17/2018
by Ryan Trisnojoyo
*/

/*

  //Sample Data Types
  
  //"U=usMQt3d&P=arG8lu&ID=4732&T=1540406017&I=5&DT=3&D=493,2834,234" 
  
  Legend

  U- Username 
  P- Password 
  ID- DeviceID
  T- Timestamp of first sample (seconds since 1970-01-01) //Unix Epoch
  I- Interval
  DT- DataTypeID
  D- Data


*/

//variable that need to be changed for every device
String DeviceID = "1789";
String timeNow = "1540406017";
String interval = "15";

// libraries
#include <MKRGSM.h>
#include <RTCZero.h>

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

/* Create an rtc object */
RTCZero rtc;

// URL, path & port (for example: arduino.cc)
char server[] = "ws.uscubed.com";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 55;
const byte hours = 23;

/* Change these values to set the current initial date */
const byte day = 12;
const byte month = 1;
const byte year = 17;

bool matched = false;

int pinRelay = 7; //relay pinMode=OUTPUT pin 7 
int pinFloat = 5; //float sensor pinMode=INPUT_PULLUP pin 5
int pinSensor = A0; //analog for ultra sensor , analog input
int sensorValue = 0;
int sensorSend = 0;
int counter = 0;
String tempSensor = "";

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  

    // connection state
  boolean connected = false;
  
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, HIGH);
  pinMode(pinFloat,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  rtc.begin();

  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(23, 56, 00);
  rtc.enableAlarm(rtc.MATCH_MMSS); // dont forget to change this 

  rtc.attachInterrupt(alarmMatch);
  rtc.standbyMode();
  
 
}

void loop()
{
if (matched) {
      // If your SIM has PIN, pass it as a parameter of begin() in quotes
  boolean connected = false;
  
  while (!connected) {
    //String dataFloat = ""; //float sensor data that will be sent via web client
   
    digitalWrite(pinRelay, LOW);
    delay(5000);
    sensorValue = analogRead(pinSensor);
    digitalWrite(pinRelay, HIGH);
    delay(100);

   if (counter == 0){
      //do nothing
      String dataSensor = "U=usMQt3d&P=arG8lu&ID=" + DeviceID;
      dataSensor += "&T=" + timeNow + "&I=" + interval + "&DT=3" + "&D=";
      dataSensor += String(sensorValue);  

      postRequest(dataSensor);
    }
    else if (counter == 1){
       tempSensor = String(sensorValue); 
    }
    else {
      tempSensor = tempSensor + "," + String(sensorValue);
    }
    
    counter++;
    
    if (counter == 5){
    String dataSensor = "U=usMQt3d&P=arG8lu&ID=" + DeviceID;
    dataSensor += "&T=" + timeNow + "&I=" + interval + "&DT=3" + "&D=";
    dataSensor += String(tempSensor);  

    postRequest(dataSensor);
    
    counter = 1;
   
    }
//    sensorValue = String("321,3123,5234");
    String dataSensor = "U=usMQt3d&P=arG8lu&ID=" + DeviceID;
    dataSensor += "&T=" + timeNow + "&I=" + interval + "&DT=3" + "&D=";
    dataSensor += String(counter);  

    postRequest(dataSensor);
    /*
    String dataSensor = "U=usMQt3d&P=arG8lu&ID=" + DeviceID;
    dataSensor += "&T=" + timeNow + "&I=" + interval + "&DT=3" + "&D=";
    dataSensor += "321,3123,5234";  
    
    counter == 0;

    postRequest(dataSensor);
    
    Serial.println("\nCOMPLETE ULTRASONIC SENSOR!\n");

    //float sensor
    String alarm = "";
    int floatStatus = digitalRead(pinFloat);
    if (floatStatus == 0){
      alarm = "1"; //TRUE
      
    } else{
      alarm = "0"; // FALSE
    }

   // String dataFloat = "U=usMQt3d&P=arG8lu&ID=" + DeviceID;
  //  dataFloat += "&T=" + timeNow + "&I=" + "&DT=7" + "D=";
  //  dataFloat += String(floatStatus);
   // postRequest(dataFloat);
    
    Serial.println("COMPLETE FLOAT SENSOR: " + floatStatus);

*/
    //set alarm every 1 hours
   /* int alarmHours = rtc.getHours();
    alarmHours += 1;
    if (alarmHours >= 24) {
      alarmHours -= 24;
    }
*/

    int alarmMinutes = rtc.getMinutes();
    alarmMinutes += 15;
    if (alarmMinutes >= 60) {
      alarmMinutes -= 60;
    }
    //Synchronize the clock
    int alarmSeconds = rtc.getSeconds() - 8;
    if(alarmSeconds <= 0){
      alarmSeconds = 0;
    }
    //Serial.println("The next alarm is: " + String(alarmHours) + " " + String(rtc.getMinutes()) + " " + String(rtc.getSeconds()));
    rtc.setAlarmTime(rtc.getHours(), alarmMinutes, alarmSeconds);
    
    /*
    //this one working
    //send every 5 minutes
    int alarmMinutes = rtc.getMinutes();
    alarmMinutes += 5;
    if (alarmMinutes >= 60) {
      alarmMinutes -= 60;
    }
    Serial.println("The next alarm is: " + String(rtc.getHours()) + " " + String(alarmMinutes) + " " + String(rtc.getSeconds()));
    rtc.setAlarmTime(rtc.getHours() , alarmMinutes, rtc.getSeconds());
    */
    
    Serial.println("SLEEP WAIT FOR THE NEXT ALARM");
    rtc.standbyMode();    // Sleep until next alarm match
    
  }
  
}
}

void alarmMatch() {
  matched = true;
}

void postRequest(String content){

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
  
 if (client.connect(server, port))
  {
    Serial.println("Connected to server");
    // Make a HTTP request
    
  client.println("POST /Arduino.aspx HTTP/1.1");
  client.println("Host: ws.uscubed.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded;");
  client.print("Content-Length: ");
  client.println(content.length());
  client.println();
  client.println(content);
  digitalWrite(LED_BUILTIN, HIGH);
  client.stop();
  } 
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

