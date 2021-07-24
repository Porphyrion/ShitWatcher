struct SimManager 
{
public:
  String sendATCommand(String cmd, bool waiting)
  {
    String _resp = "";
    Serial.println(cmd);
    gsm.println(cmd);
    if (waiting) {
      _response = waitResponse();
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

  String _response = ""; 
};