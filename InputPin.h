struct InputPin
{
  bool state = false;
  int pin;

  void setPin(int p) 
  {
    pin = p;
    pinMode(4,INPUT);
  }

  
  bool check()
  {
    int d = digitalRead(pin);
    
    if(d & !state)
      state = true;
    else if(state & !d)
      state = false;

    return state;
  }
};