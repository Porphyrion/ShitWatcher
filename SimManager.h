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
      if (_resp.startsWith(cmd)) {
        _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
      }
    }
    
    return _resp;
};


void sendSMS(String phone, String message){
    sendATCommand("AT+CMGS=\"" + phone + "\"", true);            
    sendATCommand(message + "\r\n" + (String)((char)26), true); 
};

void parseSMS(String msg) 
{
    String msgheader  = "";
    String msgbody    = "";
    String msgphone   = "";

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

    // Далее пишем логику обработки SMS-команд.
    // Здесь также можно реализовывать проверку по номеру телефона
    // И если номер некорректный, то просто удалить сообщение.
}

bool simInit()
{
    bool result = true;
    String res = "";

    res = sendATCommand("AT\r\n", true);  
    if(res != "OK")
        result = false;
    
    res = "";

    res = sendATCommand("AT+CMGF=1;&W\r\n", true);  
    if(res != "OK")
        result = false;

    return result;
}
