#ifndef INPUT_PIN
#define INPUT_PIN

struct InputPin
{
    void init() 
    {
      pinMode(pin,INPUT);
    }
    
    void checkStatus()
    {
      int d = digitalRead(pin);

      if(d & workingStatus)
      {
        workingStatus = false;
        statusChanged = true;
      }
      else if(!workingStatus & !d)
      {
        workingStatus = true;
        statusChanged = true;
      }
    }
    
    bool workingStatus = true;
    bool statusChanged = false;

    int pin;
    
    String title;
    String problemMessage;
    String resolvedMessage;
};


#endif
