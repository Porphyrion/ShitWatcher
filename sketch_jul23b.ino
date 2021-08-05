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

static const char* goingWell           = "All systems are working";
static const char* start               = "Start!";
static const char* goingBad            = "Some systems doesn`t workig";
static const char* allProblemsResolved = "All systems have returned to work!";

InputPinArray pins;
Pager pager;

void setup() {
  pins.addPin(4, "KnsOne");
  pins.addPin(7, "KnsTwo");
  pins.initPins();
  
  gsm.begin(9600);
  
  pager.phoneBook.addNumber("+79636556042");
  pager.phoneBook.addNumber("+79265527150");
  pins.pager = &pager;
  
  delay(10000);

  simInit();
  status = Status::Starting;
}


void timerEventInit()
{
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
