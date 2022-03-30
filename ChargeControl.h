#ifndef CHARGE_CONTROL
#define CHARGE_CONTROL

#include "InputPin.h"
#include "Pager.h"

class ChargeControl
{
    const String CHARGE_PROBLEM = " turned off";
    const String CHARGE_PROBLEM_RESOLVED = " turned on";
    const String CHARGE_TITLE = "Charge";

public: 
    void init(int pinId)
    {
        chargePin.pin = pinId;
        chargePin.title = CHARGE_TITLE;
        chargePin.problemMessage =  CHARGE_PROBLEM;
        chargePin.resolvedMessage =  CHARGE_PROBLEM_RESOLVED; 
        chargePin.init();
    }

    void check()
    {
        chargePin.checkStatus();

        if(!chargePin.workingStatus)
        {
        if(chargePin.statusChanged)
            _pinTurnedOff();
        }
        else if(chargePin.statusChanged)
            _pinTurnedOn();            
    }

    InputPin chargePin;
    Pager* pager;


    void _pinTurnedOff()
    {
        chargePin.statusChanged = false;
        String message = chargePin.title + chargePin.problemMessage;
        pager->sendAllSms(message);   
    }

    void _pinTurnedOn()
    {
        chargePin.statusChanged = false;
        String message = chargePin.title + chargePin.resolvedMessage;
        pager->sendAllSms(message);   
    }
};


#endif