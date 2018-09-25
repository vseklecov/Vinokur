#include "SensorsProcess.h"

#define TERMOMETR_YELLOW                           \
  {                                                \
    0x28, 0xFF, 0xB6, 0x97, 0x81, 0x16, 0x05, 0x4C \
  }

#ifdef _LM35_
// Коэффициенты перевода напряжения на LM35 в температуру analogReference
#define C5V 0.48828125   // DEFAULT on 5V Arduino boards
#define C3V3 0.322265625 // DEFAULT on 3.3V Arduino boards
#define C1V1 0.107421875 // INTERNAL on ATmega168 or ATmega328, INTERNAL1V1 on Arduino Mega
#define C2V56 0.25       // INTERNAL on ATmega8, INTERNAL2V56 on Arduino Mega
#endif

void SensorsProcess::setup()
{
  _sensors.begin();
  if (_sensors.isConnected(kubThermometer))
  {
    _sensors.setResolution(kubThermometer, 12);
  }
  if (_sensors.isConnected(defThermometer))
  {
    _sensors.setResolution(defThermometer, 12);
  }

  _sensors.setWaitForConversion(false);
  _sensors.requestTemperatures();
  _sensors.setWaitForConversion(true);

#ifdef _LM35_
  analogReference(DEFAULT); // 5.5V DEFAULT on 5V Arduino boards
  pinMode(td.pin, INPUT);
#endif

#ifdef _PRESS_
  _bmp280ready = _bmp280.begin();
#ifdef _DEBUG_
  if (!_bmp280ready)
    Serial.println("Датчик BMP280 не подключен");
#endif
#endif
}

void SensorsProcess::service()
{
  float temp;
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

#ifdef _PRESS_
  if (_bmp280ready)
  {
    temp = _bmp280.readTemperature();
    _pressure = _bmp280.readPressure();
  }
#endif
  temp = _sensors.getTempC(kubThermometer);
  if (temp == DEVICE_DISCONNECTED_C)
  {
    if (alarm_kub == 0)
      alarm_kub = millis();
    else if ((millis() - alarm_kub) > 10000)
      t_kub = 0.0;
  }
  else
  {
    t_kub = temp;
    alarm_kub = 0;
  }
  temp = _sensors.getTempC(defThermometer);
  if (temp == DEVICE_DISCONNECTED_C)
  {
    if (alarm_def == 0)
      alarm_def = millis();
    else if ((millis() - alarm_def) > 10000)
      t_def = 0.0;
  }
  else
  {
    t_def = temp;
    alarm_def = 0;
  }

  _sensors.setWaitForConversion(false);
  _sensors.requestTemperatures();
  _sensors.setWaitForConversion(true);

#ifdef _DEBUG_
  Serial.print(String(t_kub) + " " + String(t_def));
#endif

#ifdef _LM35_
  td.raw = td.raw / td.samples;
  td.samples = 1;
  td.temp = td.raw * C5V + (101325 - _pressure) * 3E-4; // Для 5В плат с коррекцией на давление
#ifdef _DEBUG_
  Serial.println(" " + String(td.temp) + " " + String(td.raw) + " " + String(td.samples));
#endif
#endif

#ifdef _DEBUG_
  Serial.println("Sensors Process: " + String(millis() - b_t));
#endif
}

float SensorsProcess::readTemp(DeviceAddress therm)
{
    float temp = _sensors.getTempC(therm);
    if (temp == DEVICE_DISCONNECTED_C)
      return DEVICE_DISCONNECTED_C;
#ifdef _PRESS_
    if (_bmp280ready)
      temp += (101325 - _pressure) * 3E-4; // Коррекция на давление
#endif
    return temp;
}

#ifdef _LM35_

void SensorsProcess::updateLM35()
{
  td.raw += analogRead(td.pin);
  td.samples++;
}

#endif

void SensorsProcess::setPressure(float p)
{
  if (p > 93000 && p < 104000)
  {
    _pressure = p;
  }
}
