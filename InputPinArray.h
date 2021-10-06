#ifndef INPUT_PIN_ARRAY
#define INPUT_PIN_ARRAY

#include "InputPin.h"
#include "Pager.h"

struct InputPinArray
{    
    String knsProblem         = " has a problem";
    String knsProblemResolved = " returned to regular mode"; 

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
                    String message = array[i].id + knsProblem;
                    array[i].statusChanged = false;
                    pager->sendAllSms(message);   
                }
            }
            else if(array[i].statusChanged)
            {
                array[i].statusChanged = false;
                String message = array[i].id +  knsProblemResolved;
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
