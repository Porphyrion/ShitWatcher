#include <SoftwareSerial.h>
#include "PDUDecoder.h"

#define DEBUG_SMS false

#define SIM800_RX 8
#define SIM800_TX 9


SoftwareSerial gsm(SIM800_TX, SIM800_RX);


String waitResponse(){
    String cmdResp = ""; 
    long timeout = millis() + 10000; 
    while (!gsm.available() && millis() < timeout)  {}; 
    
    if (gsm.available()) 
      cmdResp = gsm.readString();

    return cmdResp; 
}


String sendATCommand(String cmd, bool waiting){
    String cmdResp = ""; 
    gsm.println(cmd);
    
    if (waiting) {
      cmdResp = waitResponse();
    }
    
#ifdef DEBUG_SMS
    Serial.print(cmd + " ");
    Serial.println(cmdResp);
#endif

    int isError = cmdResp.indexOf("ERROR");
    if(isError > 0)
      cmdResp = F("ERROR");

    return cmdResp;
};


void sendSMS(String phone, String message){
    sendATCommand("AT+CMGS=\"" + phone + "\"", true);        
    delay(500);        
    sendATCommand(message + "\r\n" + (String)((char)26), true); 
};


bool simInit()
{
  bool result = true;
    
  String response = "";
  response = sendATCommand("ATE0\r\n", true);
  response.trim();
  if(response != "OK")
      result = false;
      
  response = "";
  response = sendATCommand("AT\r\n", true);  
  response.trim();
  if(response != "OK")
      result = false;

  response = "";
  response = sendATCommand("AT+CREG?\r\n", true);
  String res2 = response.substring(9,12);
  if(res2 != "0,1")
    result = false;

  sendATCommand("AT+CMEE=2\r\n", true);
  sendATCommand("AT+CMGF=1;&W\r\n", true);
  sendATCommand("AT+CPMS?\r\n", true);
  sendATCommand("AT+CSCA?\r\n", true);
  sendATCommand("AT+CPAS\r\n", true);
  sendATCommand("AT+CGMR\r\n", true);
  sendATCommand("AT+COPS?\r\n", true);

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
  delay(800); 
  // Send PDU-packet len
  String resp = sendATCommand("AT+CMGS=" + (String)PDUlen, true);
  delay(800); 
  // Send Ctrl-Z
  resp = sendATCommand(PDUPack + (String)((char)26), true);
  delay(800); 
}
