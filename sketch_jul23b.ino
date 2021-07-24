#include <SoftwareSerial.h>

#include "PhoneBook.h"
#include "InputPin.h"

#define SIM800_RX 9
#define SIM800_TX 8

SoftwareSerial gsm(SIM800_TX, SIM800_RX);

unsigned long timerEvent = 2160000;

PhoneBook phoneBook;
InputPin pin4;
InputPin pin6;

String sendATCommand(String cmd, bool waiting)
{
    String _resp = "";
    Serial.println(cmd);
    gsm.println(cmd);
    if (waiting) {
      _resp = waitResponse();
      // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
      if (_resp.startsWith(cmd)) {
        _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
      }
      Serial.println(_resp);
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
    Serial.println(sendATCommand("AT+CMGS=\"" + phone + "\"", true));            
    Serial.println(sendATCommand(message + "\r\n" + (String)((char)26), true)); 
};



void setup() {
  pin4.setPin(4);
  pin4.setPin(6);
  
  gsm.begin(4800);
  Serial.begin(4800);
  
  phoneBook.addNumber("+79265527150");
  phoneBook.addNumber("+79999878814");
  phoneBook.addNumber("+79636556042");
  
  delay(10000);
  sendATCommand("AT\r\n", true);     
  sendATCommand("AT+CMGF=1;&W\r\n", true);      
}


void sendAllSms(String text)
{
   for(int i = 0; i < PhoneBook::bookSize; ++i)
      sendSMS(phoneBook[i], text);
}

void loop() {

  if(pin4.check())
    sendAllSms("KnsOne has a problem");

  if(pin6.check())
    sendAllSms("KnsTwo has a problem");


  if(millis() < timerEvent)
  {
    if(!pin4.check() & !pin6.check())
      sendAllSms("All kns are working");
     else if(pin4.check())
      sendAllSms("KnsOne aren`t working");
     else if(pin6.check())
      sendAllSms("KnsTwo aren`t working");
     else if(pin4.check() & pin6.check())
      sendAllSms("All kns are going bad. Why you haven`t done something?");

     timerEvent = millis() + timerEvent;
  }
};
