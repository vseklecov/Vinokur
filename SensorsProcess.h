#ifndef Sensors_h
#define Sensors_h

#include <OneWire.h>
#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>
#include <ProcessScheduler.h>

#ifdef _PRESS_
#include <myBMP280.h>
#endif

class SensorsProcess : public Process
{
  public:
    SensorsProcess(Scheduler &manager, ProcPriority pr, unsigned int period, OneWire *_oneWire)
        : Process(manager, pr, period)
    {
        _sensors.setOneWire(_oneWire);
#ifdef _LM35_
        td.pin = LM35;
        td.temp = 0.0;
        td.raw = 20;
        td.samples = 1;
#endif
    }

    void setup();
    void service();

#ifdef _LM35_
    void updateLM35();
    inline float getTD() { return td.temp; }
#endif

    inline float getThempKub(void) { return t_kub; }
    inline float getThempDef(void) { return t_def; }
    inline float getPressure(void) { return _pressure; }
    inline float getThemp(void) { return temp; }

    void setPressure(float);

  private:
    DallasTemperature _sensors;
    DeviceAddress kubThermometer = {0x28, 0xFF, 0x57, 0x97, 0x81, 0x16, 0x03, 0x86}; // Термометр с синей полоской
    //DeviceAddress kubThermometer = {0x28, 0x61, 0x64, 0x12, 0x27, 0xFD, 0xBB, 0x37}; // Медный
    DeviceAddress defThermometer = {0x28, 0xFF, 0x09, 0xD5, 0x81, 0x16, 0x04, 0xD5}; // Термометр без полосок (черный)

    float _pressure; // Давление для корректировки температуры в Па
    float temp;      // Температура в помещении
    float t_kub;     // Температура куба
    float t_def;     // Температура перед холодильником

#ifdef _LM35_
    struct
    {
        int pin;
        float temp;
        unsigned long raw;
        int samples;
    } td;
#endif

#ifdef _PRESS_
    boolean _bmp280ready;
    myBMP280 _bmp280;
#endif

    float readTemp(DeviceAddress);
};

#endif
