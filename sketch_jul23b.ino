#include <SoftwareSerial.h>

#include "PhoneBook.h"
#include "InputPin.h"

#define SIM800_RX 9
#define SIM800_TX 8

enum Status
{
  Good,
  StartBad,
  Bad
};

Status status = Status::Good;
SoftwareSerial gsm(SIM800_TX, SIM800_RX);

unsigned long GoodTimerEvent  = 86400000;
unsigned long BadTimerEvent   = 0;

const unsigned long DayDelta       = 86400000;
const unsigned long SixHourstDelta = 21600000;

PhoneBook phoneBook;
InputPin pin4;

String sendATCommand(String cmd, bool waiting)
{
    String _resp = "";
    gsm.println(cmd);
    
    if (waiting) {
      _resp = waitResponse();
      // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
      if (_resp.startsWith(cmd)) {
        _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
      }
    }
    
    return _resp;
};

String waitResponse()
{
    String _resp = ""; 
    long _timeout = millis() + 10000; 
    while (!gsm.available() && millis() < _timeout)  {}; 
    
    if (gsm.available()) 
      _resp = gsm.readString();

    return _resp; 
};

  
void sendSMS(String phone, String message){
    sendATCommand("AT+CMGS=\"" + phone + "\"", true);            
    sendATCommand(message + "\r\n" + (String)((char)26), true); 
};





void setup() {
  pin4.setPin(4);
  
  gsm.begin(4800);
  Serial.begin(4800);
  
  phoneBook.addNumber("+79636556042");
  phoneBook.addNumber("+79265527150");
  
  delay(10000);
  sendATCommand("AT\r\n", true);     
  sendATCommand("AT+CMGF=1;&W\r\n", true);      
}


void sendAllSms(String text)
{
   for(int i = 0; i < phoneBook.size()+1; ++i)
   {
    sendSMS(phoneBook[i], text);
    delay(2000);
   }
}


void timerEventGood()
{
  if(millis() > GoodTimerEvent)
  {
     sendAllSms("Status good");  
     GoodTimerEvent = millis() + DayDelta;
  }
}


void startTimerEventBad()
{
  status = Status::Bad;
  BadTimerEvent = millis() + SixHourstDelta;
}


void timerEventBad()
{
  if(millis() > BadTimerEvent)
  {
     if(pin4.workingStatus)
     {
        status = Status::Good;
        BadTimerEvent = 0;
        GoodTimerEvent = millis() + DayDelta;
        sendAllSms("Problem with kns one has been resolved");
        return;
     }
         
     sendAllSms("Status bad");  
     BadTimerEvent = millis() + SixHourstDelta;
  }
}


void loop() {  
  pin4.checkStatus();
  if(!pin4.workingStatus & !pin4.smsStatus)
  {
    sendAllSms("Kns One has a problem!");
    pin4.smsStatus = true;  
    status = Status::StartBad;
  }

  switch(status)
  {
    case Status::Good:
      timerEventGood();
      break;
    case Status::Bad:
      timerEventBad();
      break;
     case Status::StartBad:
      startTimerEventBad();
      break;
     default:
      break;
  }
};
