#include <SoftwareSerial.h>

#include "PhoneBook.h"
#include "InputPin.h"

static const String numberAdded = "Number added";
static const String badThings1  = "Huston we have a problem";

#define SIM800_RX 9
#define SIM800_TX 8

SoftwareSerial gsm(SIM800_TX, SIM800_RX);

PhoneBook phoneBook;
InputPin pin4;


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
    else
      Serial.println("Timeout..."); 

    return _resp; 
  };

  
  void sendSMS(String phone, String message)
  {
    Serial.println(sendATCommand("AT+CMGS=\"" + phone + "\"", true));            
    Serial.println(sendATCommand(message + "\r\n" + (String)((char)26), true)); 
  };

  void parseSMS(String msg) {
  String msgheader  = "";
  String msgbody    = "";
  String msgphone    = "";

  msg = msg.substring(msg.indexOf("+CMGR: "));
  msgheader = msg.substring(0, msg.indexOf("\r"));

  msgbody = msg.substring(msgheader.length() + 2);
  msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK"));
  msgbody.trim();

  int firstIndex = msgheader.indexOf("\",\"") + 3;
  int secondIndex = msgheader.indexOf("\",\"", firstIndex);
  msgphone = msgheader.substring(firstIndex, secondIndex);

  Serial.println("Phone: "+msgphone);
  Serial.println("Message: "+msgbody);

  phoneBook.addNumber(msgphone);
  sendSMS(msgphone, numberAdded);
}

String _response = ""; 

void setup() {
  pin4.setPin(4);
  
  gsm.begin(4800);
  Serial.begin(4800);
  Serial.println("Start!");

  phoneBook.addNumber("+79265527150");
  
  delay(10000);
  sendATCommand("AT\r\n", true);     
  sendATCommand("AT+CMGF=1;&W\r\n", true);      
}


void loop() {

  if(pin4.check())
  {
    for(int i = 0; i < PhoneBook::bookSize; ++i)
      sendSMS(phoneBook[i], "Huston we have a problem - Lovkis Pidor");
  }
    
  
  if (gsm.available())   {
    _response = waitResponse(); 
    _response.trim();                           
    if (_response.startsWith("+CMTI:")) {      
      int index = _response.lastIndexOf(",");   
      String result = _response.substring(index + 1, _response.length()); 
      result.trim();                            
      _response = sendATCommand("AT+CMGR="+result, true); 
      parseSMS(_response);                    
      // sendATCommand("AT+CMGDA=\"DEL ALL\"", true);
    }
  }
};
