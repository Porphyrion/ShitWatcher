#ifndef INPUT_PIN_ARRAY
#define INPUT_PIN_ARRAY

#include "InputPin.h"
#include "Pager.h"
struct InputPinArray
{    
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
                {}
            }
            else if(array[i].statusChanged)
            {}
            
        }
        return result;
    }

    static const int SIZE = 2;
    unsigned int counter;
    InputPin array[SIZE];
    Pager* pager;
};


#endif
