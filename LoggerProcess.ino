#include <Arduino.h>

#include "LoggerProcess.h"

#include <SD.h>
#include <TimeLib.h>

#include "SensorsProcess.h"
extern SensorsProcess _sensors;

#include "DistillationProcess.h"
extern DistillMashProcess _distilMash;
extern FractioningProcess _refluxStill;

#include "Termostat.h"
extern TermostatProcess termostat;

#include "Functions.h"

void LoggerProcess::setup()
{
    if (!SD.begin(SD_CS))
    {
        disable();
    }
    _write_header = false;
}

void LoggerProcess::service()
{
#ifdef _DEBUG_
    unsigned long b_t = millis();
#endif

    if (termostat.isEnabled())
    {
        filenameLog = "termstat.csv";
        File dataFile = SD.open(filenameLog, FILE_WRITE);
        if (dataFile)
        {
            if (!_write_header)
            {
                dataFile.println("Термостат");
                _write_header = true;
            }
            dataFile.print(formatTime(2, 0));
            dataFile.print(";" + String(_sensors.getThempKub()));
            dataFile.print(";" + String(_sensors.getThempDef()));
            dataFile.print(";" + String(_sensors.getPressure()));
            dataFile.println(";" + termostat.getStateString());
            dataFile.close();
        }
    } else if (_distilMash.isEnabled())
    {
        filenameLog = "distmash.csv";
        File dataFile = SD.open(filenameLog, FILE_WRITE);
        if (dataFile)
        {
            switch (_distilMash.getState())
            {
            case STATE_STOPS:
                if (!_write_header)
                {
                    dataFile.println(_distilMash.getHeader());
                    _write_header = true;
                }
                break;
            case STATE_CANCEL:
                break;
            default:
                dataFile.print(formatTime(2, 0));
                dataFile.print(";" + String(_sensors.getThempKub()));
                dataFile.print(";" + String(_sensors.getThempDef()));
                dataFile.print(";" + String(_sensors.getPressure()));
                dataFile.print(";" + _distilMash.getStateString());
                if (_distilMash.boiling_kub > 0.0)
                    dataFile.println(";" + String(_distilMash.boiling_kub));
                else
                    dataFile.println(";" + String(_distilMash.temp_boiling));
            }
            dataFile.close();
        }
    } else if (_refluxStill.isEnabled())
    {
        filenameLog = "reflux.csv";
        File dataFile = SD.open(filenameLog, FILE_WRITE);
        if (dataFile)
        {
            switch (_refluxStill.getState())
            {
            case STATE_STOPS:
                if (!_write_header)
                {
                    dataFile.println(_refluxStill.getHeader());
                    _write_header = true;
                }
                break;
            case STATE_CANCEL:
                break;
            default:
                dataFile.print(formatTime(2, 0));
                dataFile.print(";" + String(_sensors.getThempKub()));
                dataFile.print(";" + String(_sensors.getThempDef()));
                dataFile.print(";" + String(_sensors.getPressure()));
                dataFile.print(";" + _refluxStill.getStateString());
                if (_refluxStill.boiling_kub > 0.0)
                    dataFile.println(";" + String(_refluxStill.boiling_kub));
                else
                    dataFile.println(";" + String(_refluxStill.temp_boiling));
            }
            dataFile.close();
        }
    }

#ifdef _DEBUG_
    Serial.println("Logger Process: " + String(millis() - b_t));
#endif
}
