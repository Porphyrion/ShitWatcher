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

static const char* initProblem         = "Module has a problem with init";
static const char* initProblemResolved = "Module has a problem with init";
static const char* goingWell           = "All systems are working";
static const char* goingWellStart      = "Start! All systems are working!";


InputPinArray pins;
Pager pager;

void setup() {
  pins.addPin(4, "KnsOne");
  pins.addPin(7, "KnsTwo");
  pins.initPins();
  
  gsm.begin(4800);
  Serial.begin(4800);
  
  pager.phoneBook.addNumber("+79636556042");
  pager.phoneBook.addNumber("+79265527150");
  pins.pager = &pager;
  
  delay(10000);

  pager.sendAllSms(initProblem);
  status = Status::Starting;
}


void timerEventInit()
{
  if(simInit()){
    pager.sendAllSms(initProblemResolved);
    status = Status::Starting;
  }
}


void timerEventStarting()
{
  if(pins.checkPins())
  {
    status = Status::Good;
    pager.sendAllSms(goingWellStart);
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
