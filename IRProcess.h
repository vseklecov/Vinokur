#ifndef __IR_PROCESS_H__
#define __IR_PROCESS_H__

#include <IRremote.h>
#include <ProcessScheduler.h>

class IRProcess: public Process
{
  public:

    enum Keys {KEY_ON, KEY_MODE, KEY_SOUND, KEY_PLAY, KEY_PREV, KEY_NEXT, KEY_EQ, KEY_MINUS, KEY_PLUS,
      KEY_ARROW, KEY_U_SD, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9};

    IRProcess(Scheduler &manager, ProcPriority pr, unsigned int period, byte pin)
      : Process(manager, pr, period)
      {
        _irrecv = new IRrecv(pin);
      }

    void setup(void);
    void service(void);
    inline void setCallback(void (*func)(char)) { _func_callback=func;}
    
  private:
    IRrecv *_irrecv;
    void (*_func_callback)(char);
    char convertNEC(unsigned long);
};


void IRProcess::setup()
{
  _irrecv->enableIRIn();
}


void IRProcess::service()
{
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif
  
  decode_results results;
  if (_irrecv->decode(&results)) {
    if (!results.overflow && _func_callback!=NULL) {
      _func_callback(convertNEC(results.value));
    }
    _irrecv->resume();
  }

#ifdef _DEBUG_
  Serial.println("IR Process: "+String(millis()-b_t));
#endif
}

char IRProcess::convertNEC(unsigned long code)
{
  switch (code)
  {
    case 0xFFA25D: // Вкл
      return '@';
    case 0xFF629D: // Mode
      return 'M';
    case 0xFFE21D: // Звук выкл
      return '\e'; // Escape
    case 0xFF22DD: // Play/pause
      return 'P';
    case 0xFF02FD: // Предыдущий
      return 'S';
    case 0xFFC23D: // Следующий
      return 'N';
    case 0xFFE01F: // EQ
      return 'Q';
    case 0xFFA857: // '-'
      return '-';
    case 0xFF906F: // '+'
      return '+';
    case 0xFF6897: // '0'
      return '0';
    case 0xFF9867: // Стрелки туда-сюда
      return '\b'; // Backspace
    case 0xFFB04F: // U/SD
      return '\n'; // Enter
    case 0xFF30CF: // '1'
      return '1';
    case 0xFF18E7: // '2'
      return '2';
    case 0xFF7A85: // '3'
      return '3';
    case 0xFF10EF: // '4'
      return '4';
    case 0xFF38C7: // '5'
      return '5';
    case 0xFF5AA5: // '6'
      return '6';
    case 0xFF42BD: // '7'
      return '7';
    case 0xFF4AB5: // '8'
      return '8';
    case 0xFF52AD: // '9'
      return '9';
    default:
      break;
  }
  return '\0';
}

#endif
