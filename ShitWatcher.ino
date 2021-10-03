#include "Pager.h"
#include "InputPinArray.h"

enum Status
{
  Init, 
  Starting,
  Good,
  Bad
};

Status status = Status::Init;
 
unsigned long GoodTimerEvent  = 7200000;
unsigned long BadTimerEvent   = 0;

const unsigned long DayDelta       = 7200000;
const unsigned long SixHourstDelta = 3600000;


InputPinArray pins;
Pager pager;

void setup(){
  pins.addPin(4, "КНС ОСНОВА");
  pins.addPin(7, "КНС ДУШ");
  pins.initPins();

  // Serial.begin(9600);
  gsm.begin(9600);
  
  pager.phoneBook.addNumber("+79636556042");
  pager.phoneBook.addNumber("+79265527150");
  pins.pager = &pager;
}


void timerEventInit()
{
  if(simInit())
  {

    status = Status::Starting;
    delay(4000);
    pager.sendAllSms(F("Модуль запущен!")); 
  }
}


void timerEventStarting()
{ 
  if(pins.checkPins())
  {
    status = Status::Good;
    pager.sendAllSms(F("Все системы работают"));
  }
  else
  {
    status = Status::Bad;
    pager.sendAllSms(F("Статус бэд!"));  
    BadTimerEvent = millis() + SixHourstDelta;
    GoodTimerEvent = 0;
    return;
  }
}


void timerEventGood()
{
  if(!pins.checkPins())
  {
    status = Status::Bad;
    BadTimerEvent = millis() + SixHourstDelta;
    GoodTimerEvent = 0;
    return;
  }
  
  if(millis() > GoodTimerEvent)
  {
     pager.sendAllSms(F("Все системы работают"));  
     GoodTimerEvent = millis() + DayDelta;
  }
}


void timerEventBad()
{
  if(pins.checkPins())
  {
    status = Status::Good;
    GoodTimerEvent = millis() + DayDelta;
    pager.sendAllSms("Система вернулась к работе");
    return;
  }

  if(millis() > BadTimerEvent)
  {    
     pager.sendAllSms(F("Статус бэд!"));  
     BadTimerEvent = millis() + SixHourstDelta;
  }
}


void loop() {
  switch(status)
  {
    case Status::Init:
      timerEventInit();
      break;
    case Status::Starting:
      timerEventStarting();
      break;
    case Status::Good:
      timerEventGood();
      break;
    case Status::Bad:
      timerEventBad();
      break;
     default:
      break;
  }
};
