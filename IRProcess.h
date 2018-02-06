#ifndef __IR_PROCESS_H__
#define __IR_PROCESS_H__

#include <IRremote.h>
#include <ProcessScheduler.h>

class IRProcess : public Process
{
public:

  IRProcess(Scheduler &manager, ProcPriority pr, unsigned int period, byte pin)
      : Process(manager, pr, period)
  {
    _irrecv = new IRrecv(pin);
  }

  void setup(void);
  void service(void);
  void setCallback(void (*func)(char)) { _func_callback = func; }
  char getch(void);

private:
  IRrecv *_irrecv;
  void (*_func_callback)(char);
  char convertNEC(unsigned long);
};

#endif