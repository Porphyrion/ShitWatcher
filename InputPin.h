struct InputPin
{
    void init() 
    {
      pinMode(pin,INPUT);
      statusChanged = false;
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
    String id;
    int pin;
};
