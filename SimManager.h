#include <SoftwareSerial.h>

struct SimManager 
{    

    String sendATCommand(String cmd, bool waiting)
    {
        String _resp = "";
        Serial.println(cmd);
        gsm.println(cmd);
        if (waiting) {
          _response = waitResponse();
          Serial.println(_resp);
        }
        return _resp;
    };

    String _waitResponse()
    {
        String _resp = ""; 
        long _timeout = millis() + 10000; 
        while (!gsm.available() && millis() < _timeout)  {}; 
    
        if (gsm.available()) 
            _resp = gsm.readString();
        else
            Serial.println("Timeout..."); 

        return _resp; 
    }

  

void sendSMS(String phone, String message)
{
    Serial.println(_sendATCommand("AT+CMGS=\"" + phone + "\"", true));            
    Serial.println(_sendATCommand(message + "\r\n" + (String)((char)26), true)); 
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

    bool init()
    {
        sendATCommand("AT\r\n", true);     
        sendATCommand("AT+CMGF=1;&W\r\n", true);    
    }

    String _response = ""; 
};