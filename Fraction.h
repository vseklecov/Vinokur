#ifndef __FRACTION_H__
#define __FRACTION_H__

#include <Time.h>
#include <ProcessScheduler.h>
#include <UTFT.h>

#include "SensorsProcess.h"

class FractionProcess;

namespace Fraction {
    const static char *states_string[8] = {
        "Стоп  ",
        "Нагрев",
        "НаСебя",
        "Головы",
        "Тело  ",
        "Хвосты",
        "Конец ",
        "Прервн"
    };
    /*const static char *param_string[3] = {
    //   01234567890123456789
        "1.Мин.темп. ",
        "2.Макс.темп ",
        "3.Готовность"
    };*/
};

class FractionScreen
{
public:
    FractionScreen(UTFT&, FractionProcess&);
    ~FractionScreen() {}

    void draw();
    void update();
    void recive(char ch);

private:
    UTFT &lcd;
    FractionProcess &fp;
    
    uint8_t x;
    uint8_t y;
};


class FractionProcess: public Process
{
    
    friend class FractionScreen;

public:

    FractionProcess(Scheduler&, ProcPriority, unsigned int, SensorsProcess&);

    void setup();
    void service();
    void recive(char ch);

    void heater_on() {};
    void heater_off() {};

    typedef enum states
    { 
        STATE_STOP = 0,
        STATE_HEAT,
        STATE_EQUIL,
        STATE_HEAD,
        STATE_HEART,
        STATE_TAILS,
        STATE_END,
        STATE_CANCEL
    } states;

    String getStateString() {return Fraction::states_string[state];}
    time_t durationState() { return now() - state_begin[state]; }
    time_t durationState(states state);

    uint8_t getTempBoiling() {return tempBoiling;}
    void setHeating(int8_t procent) { heating = procent;}
    void setCooling(int8_t procent) { cooling = procent;}
    bool getCooling() {return cooling>0;}

protected:

    void readDataEEPROM();
    void writeDataEEPROM();

    SensorsProcess &sp;

    time_t state_begin[STATE_CANCEL+1];
    time_t startBoiling;
    time_t pause;

    float tempBoiling;        // Температура закипания куба
    float tempKub;            // Текущая температура куба

    float tempKubCoolOn;
    float tempDefCoolOn;
    float tempKubDistOff;
    float tempDefDistOff;
    int  equilTime;
    float tempKubHeartOff;
    float tempDefHeartOff;

    states state;
    int8_t heating;
    int8_t cooling;
};

#endif