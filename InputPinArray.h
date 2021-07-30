#include "InputPin.h"

struct InputPinArray
{    
    void addPin(int p){
        if(counter < SIZE){
            InputPin pin;
            pin.pin = p;
            this->array[counter] = pin;
            counter++;
        }
        return;
    }

    void initPins(){
        for (int i = 0; i < SIZE; ++i)
            array[i].init();
    }
private:
    static const int SIZE = 2;
    unsigned int counter;
    InputPin array[SIZE];
};
