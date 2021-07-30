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
      }
      else if(!workingStatus & !d)
      {
        workingStatus = true;
        smsStatus = false;
      }
    }
    
    bool workingStatus = true;
    bool smsStatus = false;
    int pin;
};
