#ifndef INPUT_PIN_ARRAY
#define INPUT_PIN_ARRAY

#include "InputPin.h"
#include "Pager.h"

struct InputPinArray
{    
    void addPin(int p, String title, String pMessage, String& rMessage){
        if(counter < SIZE){
            InputPin pin;
            pin.pin = p;
            pin.title = title;
            pin.problemMessage = pMessage;
            pin.resolvedMessage = rMessage; 
            this->array[counter] = pin;
            ++counter;
        }
        return;
    }

    void initPins(){
        for (int i = 0; i < SIZE; ++i)
            array[i].init();
    }

    bool checkPins()
    {
        bool result = true;
        for (int i = 0; i < SIZE; ++i)
        {
            InputPin& pin = array[i];
            pin.checkStatus();

            if(!pin.workingStatus)
            {
                result = false;
                if(pin.statusChanged)
                    pinTurnedOff(pin);
            }
            else if(pin.statusChanged)
                pinTurnedOn(pin);            
        }
        return result;
    }
 
    void pinTurnedOff(InputPin& pin)
    {
        String message = pin.title + pin.problemMessage;
        pin.statusChanged = false;
        pager->sendAllSms(message);   
    }

    void pinTurnedOn(InputPin& pin)
    {
        pin.statusChanged = false;
        String message = pin.title + pin.resolvedMessage;
        pager->sendAllSms(message);   
    }

    static const int SIZE = 2;
    unsigned int counter = 0;
    InputPin array[SIZE];
    Pager* pager;
};


#endif
