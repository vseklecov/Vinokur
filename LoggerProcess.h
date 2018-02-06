#ifndef __LOGGER_PROCESS_H__
#define __LOGGER_PROCESS_H__

#include <ProcessScheduler.h>

class LoggerProcess : public Process
{
public:
  LoggerProcess(Scheduler &manager, ProcPriority pr, unsigned int period)
      : Process(manager, pr, period)
  {
    filenameLog = "datalog.csv";
    _write_header = false;
  }

  void setup(void);
  void service(void);

private:
  String filenameLog;
  boolean _write_header;
};

#endif