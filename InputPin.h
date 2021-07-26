struct InputPin
{
    bool workingStatus = true;
    bool smsStatus = false;
    int pin;

    void setPin(int p) 
    {
      pin = p;
      pinMode(4,INPUT);
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
};
