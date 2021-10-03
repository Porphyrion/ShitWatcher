#ifndef PAGER
#define PAGER

#include "PhoneBook.h"
#include "SimManager.h"

struct Pager
{
    PhoneBook phoneBook;

    void sendAllSms(String text)
    {   
        for(int i = 0; i < phoneBook.size(); ++i)
        {
         sendSMSinPDU(phoneBook[i], text);
         delay(3000);
        }
    }
};

#endif
