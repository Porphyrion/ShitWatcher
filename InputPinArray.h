#ifndef INPUT_PIN_ARRAY
#define INPUT_PIN_ARRAY

#include "InputPin.h"
#include "Pager.h"

struct InputPinArray
{    
    String knsProblem         = "Проблемы с ";
    String knsProblemResolved = "Проблемы решены с "; 

    void addPin(int p, String&& id){
        if(counter < SIZE){
            InputPin pin;
            pin.pin = p;
            pin.id = id;
            this->array[counter] = pin;
            counter++;
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
            array[i].checkStatus();
            if(!array[i].workingStatus){
                result = false;
                if(array[i].statusChanged)
                {
                    String message = knsProblem + array[i].id;
                    array[i].statusChanged = false;
                    pager->sendAllSms(message);   
                }
            }
            else if(array[i].statusChanged)
            {
                array[i].statusChanged = false;
                String message = knsProblemResolved + array[i].id;
                pager->sendAllSms(message);   
            }
            
        }
        return result;
    }

    static const int SIZE = 2;
    unsigned int counter = 0;
    InputPin array[SIZE];
    Pager* pager;
};


#endif
