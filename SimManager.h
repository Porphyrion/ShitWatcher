#include <SoftwareSerial.h>

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
    gsm.println(cmd);
    
    if (waiting) {
      _resp = waitResponse();
    }
    
    return _resp;
};


void sendSMS(String phone, String message){
    sendATCommand("AT+CMGS=\"" + phone + "\"", true);    
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
    

    res = "";
    res = sendATCommand("AT+CMGF=1;&W\r\n", true);
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
