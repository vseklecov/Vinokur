#include <Arduino.h>

#include "LoggerProcess.h"

#include <SD.h>
#include <TimeLib.h>

#include "SensorsProcess.h"
extern SensorsProcess _sensors;

#include "DistillationProcess.h"
extern DistillMashProcess _distilMash;
extern FractioningProcess _refluxStill;

extern String formatTime(byte, time_t);

void LoggerProcess::setup()
{
    if (!SD.begin(SD_CS))
    {
        disable();
    }
}

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
    else
    {
        _write_header = false;
        return;
    }

    filenameLog = "distmash.csv";
    File dataFile = SD.open(filenameLog, FILE_WRITE);
    if (dataFile)
    {
        switch (_still->getState())
        {
        case STATE_STOPS:
            if (!_write_header)
            {
                dataFile.println(_still->getHeader());
                _write_header = true;
            }
            break;
        //case STATE_END:
        case STATE_CANCEL:
            break;
        default:
        {
            dataFile.print(formatTime(2, 0));
#ifdef _LM35_
            dataFile.print(";" + String(_sensors.getTD()));
#endif
            dataFile.print(";" + String(_sensors.getThempKub()));
            dataFile.print(";" + String(_sensors.getThempDef()));
#ifdef _PRESS_
            dataFile.print(";" + String(_sensors.getPressure()));
#endif
            dataFile.print(";" + _still->getStateString());
            if (_still->boiling_kub > 0.0)
                dataFile.println(";" + String(_still->boiling_kub));
            else
                dataFile.println(";" + String(_still->temp_boiling));
        }
        }
        dataFile.close();
    }

#ifdef _DEBUG_
    Serial.println("Logger Process: " + String(millis() - b_t));
#endif
}
