#include "Pager.h"
#include "InputPinArray.h"

enum Status
{
  Init, 
  Starting,
  Good,
  StartBad,
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


String knsProblem = " has a problem!";
String knsProblemResolved = "Problem has resolved for "; 

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

  if(!simInit())
    pager.sendAllSms(initProblem);
  else
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
    pager.sendAllSms(goingWell);
  else
  {
    //
    // Send sms with pin id who doesn`t work
    // 
  }
}


void timerEventGood()
{
  if(!pins.checkPins())
  {
    status = Status::Bad;
    //
    // Send sms with pin id who doesn`t work
    // 
    return;
  }
  
  if(millis() > GoodTimerEvent)
  {
     pager.sendAllSms("Status good");  
     GoodTimerEvent = millis() + DayDelta;
  }
}


void timerEventStartBad()
{
  status = Status::Bad;
  BadTimerEvent = millis() + SixHourstDelta;
  GoodTimerEvent = 0;
}


void timerEventBad()
{
  if(pins.checkPins())
  {
    status = Status::Good;
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
      break;
    case Status::Good:
      timerEventGood();
      break;
    case Status::Bad:
      timerEventBad();
      break;
     case Status::StartBad:
      timerEventStartBad();
      break;
     default:
      break;
  }
};
