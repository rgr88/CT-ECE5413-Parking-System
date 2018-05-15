#include <SoftwareSerial.h>
#include <String.h>
#include <Wire.h>
SoftwareSerial SIM900(7, 8);

int ind1;
int indPlus;
int indNewLine;
int indOk;
int indPhone;
unsigned long lastUpdate;
bool registered = false;
bool messageSent = false;
bool initialized = false;
bool acked = false;
bool sendButtonPressed = false;
bool ackButtonPressed = false;
String reginfo; 
String networkConnect;
String phoneNumber;
String checkMessageStatus;
String msg;
String incomingSms;

int fromHuzzahPin = 13;
int parkingAvailable = 0;
int lastTextSent = 0; //1 if last text sent said parking was available, 0 if last was saying lot full

void setup() {
  SIM900.begin(19200);
  Serial.begin(19200);
  pinMode(fromHuzzahPin, INPUT);
}

void loop() {
  if (!initialized) {
    initialization();
  }
  if (SIM900.available() > 2) {
    msg = SIM900.readString();
    indPlus = msg.indexOf('+');
    msg = msg.substring(indPlus,indPlus+5);
    Serial.println(msg);
  }
  parkingAvailable = digitalRead(fromHuzzahPin);
  Serial.println("parkingAvailable: " + (String)parkingAvailable);
  Serial.println("lastTextSent: " + (String)lastTextSent);
  if ((parkingAvailable && !lastTextSent) || (!parkingAvailable && lastTextSent)){
    sendState();
  }
  delay(1000);
}


void initialization() {  
  SIM900.print("AT+CMGF=1");                                           
  delay(100);
  SIM900.println("ATE0");                         
  delay(100);
  regLoop();
}

void regLoop() {
  while (!registered) 
  {
    SIM900.println("AT + CREG?");                         
    delay(100);
    reginfo = SIM900.readString();
    ind1 = reginfo.indexOf('1');
    networkConnect = reginfo.substring(ind1,ind1+3);
    if (networkConnect == "1,1")
    {
      initialized = true;
      registered = true;
      }
    }
}

void sendState(){
  messageSent = false;
  SIM900.println("AT + CMGS = \"+19083002699\"");                         
  delay(100);
  if(lastTextSent == 1){ //case where parking is no longer available
    SIM900.println("Sorry, parking lot full :(");    
    lastTextSent = 0;
  }
  else{ //case where parking is available
    SIM900.println("Space available, go go go!!!");   
    lastTextSent = 1;
  }                                    
  delay(100);
  SIM900.println((char)26);                                              
  delay(100);
  SIM900.println();
  delay(5000);
}


