#include <SoftwareSerial.h>
#include <cppQueue.h>
#include "PDUDecoder.h"


#define  IMPLEMENTATION  FIFO

#define SIM800_RX 9
#define SIM800_TX 8

struct SmsData
{
  String phone;
  String msg;
};

SoftwareSerial gsm(SIM800_TX, SIM800_RX);

cppQueue  messagesQueue(sizeof(SmsData), 10, IMPLEMENTATION);

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



String getDAfield(String *phone, bool fullnum) {
  String result = "";
  for (int i = 0; i <= (*phone).length(); i++) {  // Оставляем только цифры
    if (isDigit((*phone)[i])) {
      result += (*phone)[i];
    }
  }
  int phonelen = result.length();                 // Количество цифр в телефоне
  if (phonelen % 2 != 0) result += "F";           // Если количество цифр нечетное, добавляем F

  for (int i = 0; i < result.length(); i += 2) {  // Попарно переставляем символы в номере
    char symbol = result[i + 1];
    result = result.substring(0, i + 1) + result.substring(i + 2);
    result = result.substring(0, i) + (String)symbol + result.substring(i);
  }

  result = fullnum ? "91" + result : "81" + result; // Добавляем формат номера получателя, поле PR
  result = byteToHexString(phonelen) + result;    // Добавляем длиу номера, поле PL

  return result;
}



void getPDUPack(String *phone, String *message, String *result, int *PDUlen)
{
  // Поле SCA добавим в самом конце, после расчета длины PDU-пакета
  *result += "01";                                // Поле PDU-type - байт 00000001b
  *result += "00";                                // Поле MR (Message Reference)
  *result += getDAfield(phone, true);             // Поле DA
  *result += "00";                                // Поле PID (Protocol Identifier)
  *result += "08";                                // Поле DCS (Data Coding Scheme)
  //*result += "";                                // Поле VP (Validity Period) - не используется

  String msg = StringToUCS2(*message);            // Конвертируем строку в UCS2-формат
  
  *result += byteToHexString(msg.length() / 2);   // Поле UDL (User Data Length). Делим на 2, так как в UCS2-строке каждый закодированный символ представлен 2 байтами.
  *result += msg;

  *PDUlen = (*result).length() / 2;               // Получаем длину PDU-пакета без поля SCA
  *result = "00" + *result;                       // Добавляем поле SCA
}


void sendSMSinPDU(String phone, String message)
{
  Serial.println("Отправляем сообщение: " + message + " на номер " + phone);

  // ============ Подготовка PDU-пакета =============================================================================================
  // В целях экономии памяти будем использовать указатели и ссылки
  String *ptrphone = &phone;                                    // Указатель на переменную с телефонным номером
  String *ptrmessage = &message;                                // Указатель на переменную с сообщением

  String PDUPack;                                               // Переменная для хранения PDU-пакета
  String *ptrPDUPack = &PDUPack;                                // Создаем указатель на переменную с PDU-пакетом

  int PDUlen = 0;                                               // Переменная для хранения длины PDU-пакета без SCA
  int *ptrPDUlen = &PDUlen;                                     // Указатель на переменную для хранения длины PDU-пакета без SCA

  getPDUPack(ptrphone, ptrmessage, ptrPDUPack, ptrPDUlen);      // Функция формирующая PDU-пакет, и вычисляющая длину пакета без SCA

  Serial.println("PDU-pack: " + PDUPack);
  //Serial.println("PDU length without SCA:" + (String)PDUlen);

  // ============ Отправка PDU-сообщения ============================================================================================
  sendATCommand("AT+CMGF=0", true);                             // Включаем PDU-режим
  
  String resp = sendATCommand("AT+CMGS=" + (String)PDUlen, true);             // Отправляем длину PDU-пакета
  
  resp = sendATCommand(PDUPack + (String)((char)26), true);            // После PDU-пакета отправляем Ctrl+Z
  if(resp == "ERROR"){
        Serial.println("Error found");
        SmsData sms;
        sms.phone = phone;
        sms.msg = message;
        messagesQueue.push(&sms);
        return;
    }
}


void checkQueue()
{
   if(!messagesQueue.isEmpty())
   {
      int size = messagesQueue.getCount();
      Serial.println("Очко товарища" + size);
      for(int i = 0; i < size; ++i){
        SmsData sms;
        messagesQueue.pop(&sms);
        sendSMSinPDU(sms.phone, sms.msg); 
      }
   }
}
