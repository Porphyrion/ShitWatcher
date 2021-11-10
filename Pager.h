#ifndef PAGER
#define PAGER

#include "PhoneBook.h"
#include "SimManager.h"

struct Pager
{
    PhoneBook phoneBook;

    void sendAllSms(String text)
    {   
        sendATCommand("AT+CMGDA=\"DEL ALL\"\r\n", true);
        delay(2000);
        for(int i = 0; i < phoneBook.size(); ++i)
        {
         sendSMSinPDU(phoneBook[i], text);
         delay(4000);
        }
    }
};

#endif
