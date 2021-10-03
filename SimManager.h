#include <SoftwareSerial.h>
#include "PDUDecoder.h"


#define SIM800_RX 9
#define SIM800_TX 8


SoftwareSerial gsm(SIM800_TX, SIM800_RX);


String waitResponse(){
    String _resp = ""; 
    long _timeout = millis() + 10000; 
    while (!gsm.available() && millis() < _timeout)  {}; 
    
    if (gsm.available()) 
      _resp = gsm.readString();

    return _resp; 
}


String sendATCommand(String cmd, bool waiting){
    String _resp = "";
    Serial.println("Команда " + cmd);
    gsm.println(cmd);
    
    if (waiting) {
      _resp = waitResponse();
    }
    
    int isError = _resp.indexOf("ERROR");
    if(isError > 0)
      _resp = F("ERROR");
    
    Serial.println(_resp);
    return _resp;
};


void sendSMS(String phone, String message){
    String resp = sendATCommand("AT+CMGS=\"" + phone + "\"", true);        
    delay(500);        
    sendATCommand(message + "\r\n" + (String)((char)26), true); 
};


bool simInit()
{
    bool result = true;
    
    String res = "";
    res=sendATCommand("ATE0\r\n", true);
    res.trim();
    if(res != "OK")
        result = false;

    res = "";
    res = sendATCommand("AT\r\n", true);  
    res.trim();
    if(res != "OK")
        result = false;

    return result;
    res = "";
    res=sendATCommand("AT+CREG?\r\n", true);
    String res2 = res.substring(9,12);
    if(res2 != "0,1")
      result = false;
    

   return result;
}


String getDAfield(String& phone, bool fullnum) {
  String result = "";
  for (int i = 0; i <= phone.length(); i++) {
    if (isDigit(phone[i])) {
      result += phone[i];
    }
  }
  int phonelen = result.length();
  if (phonelen % 2 != 0) result += "F";           // Если количество цифр нечетное, добавляем F

  for (int i = 0; i < result.length(); i += 2) {  // Попарно переставляем символы в номере
    char symbol = result[i + 1];
    result = result.substring(0, i + 1) + result.substring(i + 2);
    result = result.substring(0, i) + (String)symbol + result.substring(i);
  }

  result = fullnum ? "91" + result : "81" + result; // Добавляем формат номера получателя, поле PR
  result = byteToHexString(phonelen) + result;    // Добавляем длиyу номера, поле PL

  return result;
}


void getPDUPack(String& phone, String& message, String& result, int& PDUlen)
{
  // PDU-type
  result += "01";
  // Message Reference
  result += "00";
  // DA field
  result += getDAfield(phone, true);
  // PID (Protocol Identifier)
  result += "00";
  //DCS (Data Coding Scheme)
  result += "08";
  // VP (Validity Period) - don`t use
  //*result += "";
  // Convert to UCS2-format
  String msg = StringToUCS2(message);
  // UDL (User Data Length)
  result += byteToHexString(msg.length() / 2);
  result += msg;
  // Packet len witout SCA
  PDUlen = result.length() / 2;
  // Result 
  result = "00" + result; 
}


void sendSMSinPDU(String phone, String message)
{
  String PDUPack; 
  int PDUlen = 0;

  getPDUPack(phone, message, PDUPack, PDUlen);

  // Turn on PDU-mode
  sendATCommand("AT+CMGF=0", true);
  // Send PDU-packet len
  String resp = sendATCommand("AT+CMGS=" + (String)PDUlen, true);
  // Send Ctrl-Z
  resp = sendATCommand(PDUPack + (String)((char)26), true);
}
