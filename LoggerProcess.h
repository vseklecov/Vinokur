#ifndef __LOGGER_PROCESS_H__
#define __LOGGER_PROCESS_H__

#include <SD.h>
#include <ProcessScheduler.h>
#include <TimeLib.h>

#include "SensorsProcess.h"
#include "DistillationProcess.h"

extern SensorsProcess _sensors;
extern DistillMashProcess _distilMash;
extern FractioningProcess _refluxStill;

extern String formatTime(byte, time_t);

class LoggerProcess: public Process
{
  public:
    LoggerProcess(Scheduler &manager, ProcPriority pr, unsigned int period)
      : Process(manager, pr, period)
      {
        filenameLog = "datalog.csv";
        _write_header = false;
      }
      
    void setup(void) {}
    void service(void);
    
  private:
    String filenameLog;
    boolean _write_header;  
};


void LoggerProcess::service()
{
  DistillationProcess *_still;
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

  if (_distilMash.isEnabled())
    _still = &_distilMash;
  else if (_refluxStill.isEnabled())
    _still = &_refluxStill;
  else {
    _write_header=false;
    return;
  }
    
  filenameLog = "distmash.csv";
  File dataFile = SD.open(filenameLog, FILE_WRITE);
  if (dataFile) {
    switch (_still->getState())
    {
      case STATE_STOPS:
        if (!_write_header) {
          dataFile.println(_still->getHeader());
          _write_header=true;
        }
        break;
      //case STATE_END:
      case STATE_CANCEL:
        break;
      default:
      {
        dataFile.print(formatTime(2,0));
#ifdef _LM35_ 
        dataFile.print(";"+String(_sensors.getTD()));
#endif
        dataFile.print(";"+String(_sensors.getThempKub()));
        dataFile.print(";"+String(_sensors.getThempDef()));
#ifdef _PRESS_
        dataFile.print(";"+String(_sensors.getPressure()));
#endif
        dataFile.print(";"+_still->getStateString());
        if (_still->boiling_kub>0.0)
          dataFile.println(";"+String(_still->boiling_kub));
        else
          dataFile.println(";"+String(_still->temp_boiling));
      }
    }
    dataFile.close();
  }

#ifdef _DEBUG_
  Serial.println("Logger Process: "+String(millis()-b_t));
#endif
}

#endif
