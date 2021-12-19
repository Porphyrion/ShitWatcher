#ifndef INPUT_PIN_ARRAY
#define INPUT_PIN_ARRAY

#include "InputPin.h"
#include "Pager.h"

struct InputPinArray
{    
    String knsProblem         = " требует вмешательства";
    String knsProblemResolved = " вернулась в работу в штатном режиме"; 
    String chargeProblem = " выключено";
    String chargeProblemResolved = " включилось";

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
                    String message{};
                    if(array[i].id == "ПИТАНИЕ")
                        message = array[i].id + chargeProblem;
                    else
                        message = array[i].id + knsProblem;

                    array[i].statusChanged = false;
                    pager->sendAllSms(message);   
                }
            }
            else if(array[i].statusChanged)
            {           
                String message{};
                if(array[i].id == "ПИТАНИЕ")
                    message = array[i].id + chargeProblemResolved;
                else
                    message = array[i].id + knsProblemResolved;
                
                array[i].statusChanged = false;
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
