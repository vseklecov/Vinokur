#ifndef __TERMOSTAT_H__
#define __TERMOSTAT_H__

#include <time.h>
#include <ProcessScheduler.h>
#include <UTFT.h>

#include "Publisher.h"
#include "SensorsProcess.h"

class TermostatProcess;

namespace Termostat {
    const static char *states_string[5] = {
        "Стоп  ",
        "Нагрев",
        "Варка ",
        "Конец ",
        "Прервн"
    };
    const static char *param_string[3] = {
    //   01234567890123456789
        "1.Мин.темп. ",
        "2.Макс.темп ",
        "3.Готовность"
    };
};

class TermostatScreen
{
public:
    TermostatScreen(UTFT&, SensorsProcess&, TermostatProcess&);
    ~TermostatScreen() {}

    void draw();
    void update();
    void recive(char ch);

    void setShowParam(bool n) {showParams = n; }
    
private:
    UTFT &lcd;
    SensorsProcess &sp;
    TermostatProcess &tp;
    
    uint8_t x;
    uint8_t y;
    bool showParams;
};


class TermostatProcess: public Process
{
    
    friend class TermostatScreen;

public:

    TermostatProcess(Scheduler&, ProcPriority, unsigned int, SensorsProcess&);

    void setup();
    void service();
    void recive(char ch);

    void heater_on() {};
    void heater_off() {};

    typedef enum states
    { 
        STOP = 0,
        HEAT,
        COOKING,
        END,
        CANCEL
    } states;

    String getStateString() {return Termostat::states_string[state];}
    time_t durationState() { return now() - state_begin[state]; }
    time_t durationState(states state);

    uint8_t getCookingEnd() {return tempProbeCookingEnd;}
    uint8_t getCookingLow() {return tempKubCookingLow;}
    uint8_t getCookingHigh() {return tempKubCookingHigh;}

protected:
    SensorsProcess &sp;

    time_t state_begin[5];
    uint8_t tempKubBegin;
    uint8_t tempKubCookingLow;
    uint8_t tempKubCookingHigh;
    uint8_t tempProbeCookingEnd;
    states state;
};

#endif