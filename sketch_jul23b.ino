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
 
unsigned long GoodTimerEvent  = 86400000;
unsigned long BadTimerEvent   = 0;

const unsigned long DayDelta       = 86400000;
const unsigned long SixHourstDelta = 21600000;

static const char* goingWell           = "Все системы работают штатно";
static const char* start               = "Старт! 31 августа!";
static const char* goingBad            = "Статус бэд!";
static const char* allProblemsResolved = "Все системы работают штатно. Снова.";

InputPinArray pins;
Pager pager;

void setup() {
  pins.addPin(4, "КНС ОСНОВА");
  pins.addPin(7, "КНС ДУШ");
  pins.initPins();

  Serial.begin(9600);
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
    pager.sendAllSms(F("Модуль запущен!"));
    
  }
}


void timerEventStarting()
{
  
  pager.sendAllSms(start);
  if(pins.checkPins())
  {
    status = Status::Good;
    pager.sendAllSms(goingWell);
  }
  else
  {
    status = Status::Bad;
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
     pager.sendAllSms(goingWell);  
     GoodTimerEvent = millis() + DayDelta;
  }
}

void timerEventBad()
{
  if(pins.checkPins())
  {
    status = Status::Good;
    GoodTimerEvent = millis() + DayDelta;
    pager.sendAllSms(allProblemsResolved);
    return;
  }

  if(millis() > BadTimerEvent)
  {    
     pager.sendAllSms("Status bad");  
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
