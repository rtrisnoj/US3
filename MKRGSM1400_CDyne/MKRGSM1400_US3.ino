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

#include <RTCZero.h>
#include <MKRGSM.h>

/* Create an rtc object */
RTCZero rtc;

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


const char PINNUMBER[] = "";

//initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

void setup() {
    // initialize serial communications and wait for port to open:
  //Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
    Serial.println("SMS Messages Sender");

  // connection state
  boolean connected = false;
  
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, HIGH);
  pinMode(pinFloat,INPUT_PULLUP);

  rtc.begin();

  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(23, 56, 00);
  rtc.enableAlarm(rtc.MATCH_HHMMSS); // dont forget to change this 

  rtc.attachInterrupt(alarmMatch);
  rtc.standbyMode();
}

void loop() {
  if (matched) {
      // If your SIM has PIN, pass it as a parameter of begin() in quotes
      boolean connected = false;
  
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

    Serial.println("GSM initialized");
    
    matched = false;

    Serial.print("Enter a mobile number: ");
    //CDyne = "7572046171";
    //Orbcomm = "53340210";
    
    char remoteNum[20] = "7572046171";  // telephone number to send sms
    Serial.println(remoteNum);

    digitalWrite(pinRelay, LOW);
    delay(7000);
    sensorValue = analogRead(pinSensor);
    digitalWrite(pinRelay, HIGH);
    delay(1000);

  
    
    // sms text
    Serial.print("Now, enter SMS content: ");
    String txtMsg = "{\"Type\":\"3\",\"Data\":" + String(sensorValue) + "}";
    Serial.println("SENDING");
    Serial.println();
    Serial.println("Message:");
    Serial.println(txtMsg);

    // send the message
    sms.beginSMS(remoteNum);
    sms.print(txtMsg);
    sms.endSMS();
    Serial.println("\nCOMPLETE ULTRASONIC SENSOR!\n");

    //float sensor
    String alarm = "";
    int floatStatus = digitalRead(pinFloat);
    if (floatStatus == 0){
      alarm = "1"; //TRUE
    } else{
      alarm = "0"; // FALSE
    }
    Serial.println("COMPLETE FLOAT SENSOR: " + floatStatus);
    
     if (floatStatus == HIGH){
      String data = "{\"Type\":\"7\",\"Data\":" + alarm + "}";
      sms.beginSMS(remoteNum);
      sms.print(data);
      sms.endSMS();
     } else {
      String data = "{\"Type\":\"7\",\"Data\":" + alarm + "}";
      sms.beginSMS(remoteNum);
      sms.print(data);
      sms.endSMS();;
     }


    //set alarm every 1 hours
    int alarmHours = rtc.getHours();
    alarmHours += 1;
    if (alarmHours >= 24) {
      alarmHours -= 24;
    }

    //Synchronize the clock
    int alarmSeconds = rtc.getSeconds() - 8;
    if(alarmSeconds <= 0){
      alarmSeconds = 0;
    }
    //Serial.println("The next alarm is: " + String(alarmHours) + " " + String(rtc.getMinutes()) + " " + String(rtc.getSeconds()));
    rtc.setAlarmTime(alarmHours, rtc.getMinutes(), alarmSeconds);
    
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
    gsmAccess.shutdown();
    Serial.println("SLEEP WAIT FOR THE NEXT ALARM");
    rtc.standbyMode();    // Sleep until next alarm match
    
  }
}

void alarmMatch() {
  matched = true;
}
