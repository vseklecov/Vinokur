#ifndef Sensors_h
#define Sensors_h

#include <OneWire.h>
#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>
#include <ProcessScheduler.h>

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

#ifdef _PRESS_
#include <BMP280.h>
#endif

class SensorsProcess: public Process
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
    inline float getTD() {return td.temp;}
#endif
    
    inline float getThempKub(void) {return t_kub;}
    inline float getThempDef(void) {return t_def;}
    inline float getPressure(void) { return _pressure;}

    void setPressure(float);
    
  private:
      
    DallasTemperature _sensors;
    DeviceAddress kubThermometer = {0x28, 0xFF, 0x57, 0x97, 0x81, 0x16, 0x03, 0x86}; // Термометр с синей полоской 
    DeviceAddress defThermometer = {0x28, 0xFF, 0x09, 0xD5, 0x81, 0x16, 0x04, 0xD5}; // Термометр без полосок (черный)
    
    float _pressure;  // Давление для корректировки температуры в Па
    float t_kub;      // Температура куба
    float t_def;      // Температура перед холодильником

#ifdef _LM35_
    struct {
      int pin;
      float temp;
      unsigned long raw;
      int samples;
    } td;
#endif

#ifdef _PRESS_
    boolean _bmp280ready;
    BMP280 _bmp280;
#endif
    
    float readTemp(DeviceAddress);
  
};

#endif
