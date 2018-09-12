#include "Termostat.h"

#include <UTFT.h>

//#include "DisplayProcess.h"
#include "Functions.h"
#include "Keys.h"

TermostatScreen::TermostatScreen(UTFT &_lcd, SensorsProcess &_sp, TermostatProcess &_tp)
    :lcd(_lcd), sp(_sp), tp(_tp)
{
    x = lcd.getFontXsize();
    y = lcd.getFontYsize();

    showParams = false;
}

/* Шрифт Tahoma1251 24х32 экран 320x480
  01234567890123456789
  --------------------
0|      Термостат     |
1|Нагрев 00:00:00     |
2| Куб 000.0          |
3| Щуп 000.0          |
4|                    |
5|                    |
6|                    |
7|                    |
8|                    |
9|00:00   00:00  000мм|
  --------------------
  01234567890123456789
*/
void TermostatScreen::draw()
{
    lcd.clrScr();
    lcd.setColor(VGA_AQUA);
    lcd.print((char*)"Термостат", CENTER, 0);
    lcd.print((char*)"Куб", x, 2*y);
    lcd.print((char*)"Щуп", x, 3*y);
    lcd.print((char*)"мм", 18*x, 9*y);
}

void TermostatScreen::update()
{
    lcd.setColor(VGA_AQUA);
    lcd.print(tp.getStateString(), 0, y);
    lcd.print(formatTime(2, tp.durationState()), x*7, y);
    lcd.printNumF(sp.getThempKub(), 1, x*5, y*2, '.', 5);
    lcd.printNumF(sp.getThempDef(), 1, x*5, y*3, '.', 5);

    time_t duration = tp.durationState(TermostatProcess::COOKING);
    if (duration > 0)
        lcd.print(formatTime(3, duration), 0, y*9);
    lcd.print(formatTime(3, 0), CENTER, y * 9);
    lcd.printNumI(round(sp.getPressure() * 7.5E-3), x*15, y*9, 3);
}


void TermostatScreen::recive(char ch)
{
    switch (ch)
    {
        case PROCESS_SETTINGS:
            showParams = ~showParams;
            if (showParams)
            {
                lcd.print(Termostat::param_string[0], 0, 5*y);
                lcd.printNumI(tp.tempKubCookingLow, 13*x, 5*y, 2);
                lcd.print(Termostat::param_string[1], 0, 6*y);
                lcd.printNumI(tp.tempKubCookingHigh, 13*x, 6*y, 2);
                lcd.print(Termostat::param_string[2], 0, 7*y);
                lcd.printNumI(tp.tempProbeCookingEnd, 13*x, 7*y, 2);
            } else {
                word fcolor = lcd.getColor();
                lcd.setColor(lcd.getBackColor());
                lcd.fillRect(0, 5*y, lcd.getDisplayXSize(), 8*y);
                lcd.setColor(fcolor);
            };
            break;
        case KEY_1:
        case KEY_2:
        case KEY_3:
            if (showParams)
            {

            }
            break;
        default:
            break;
    }
}

TermostatProcess::TermostatProcess(Scheduler &manager, ProcPriority pr, unsigned int period, SensorsProcess &_sp)
    : Process(manager, pr, period), sp(_sp)
{
    for (int i=0; i<5; i++)
        state_begin[i] = 0;
    tempKubCookingLow = 70;
    tempKubCookingHigh = 80;
    tempProbeCookingEnd = 72;
}

void TermostatProcess::setup()
{
    state = TermostatProcess::STOP;
    state_begin[state] = now();
    tempKubBegin = sp.getThempKub();
    heater_off();
}

void TermostatProcess::service()
{
    switch (state)
    {
        case TermostatProcess::STOP:
            if ((sp.getThempKub()-tempKubBegin) > 5)
            {
                state = TermostatProcess::HEAT;
                if (state_begin[state] == 0)
                    state_begin[state] = now();
            }
            break;
        case TermostatProcess::HEAT:
            heater_on();
            if (sp.getThempKub() > tempKubCookingLow)
            {
                state = TermostatProcess::COOKING;
                if (state_begin[state] == 0)
                    state_begin[state] = now();
            }
            break;
        case TermostatProcess::COOKING:
            if (sp.getThempKub() >= tempKubCookingHigh)
                heater_off();
            if (sp.getThempKub() <= tempKubCookingLow)
                heater_on();
            if (sp.getThempDef() >= tempProbeCookingEnd)
            {
                state = TermostatProcess::END;
                state_begin[state] = now();
            }
            break;
        case TermostatProcess::CANCEL:
        case TermostatProcess::END:
            heater_off();
        default:
            break;
    };
}

void TermostatProcess::recive(char ch)
{
    switch (ch)
    {
        case PROCESS_CANCEL:
            if (state == TermostatProcess::CANCEL)
                disable();
            else
                state = TermostatProcess::CANCEL;
            break;
        case PROCESS_START:
            if (state == TermostatProcess::STOP || state == TermostatProcess::CANCEL || state == TermostatProcess::END)
                state = TermostatProcess::HEAT;
            break;
        case PROCESS_PREV_STATE:
            switch (state)
            {
                case TermostatProcess::HEAT:
                    state = TermostatProcess::STOP;
                    break;
                case TermostatProcess::COOKING:
                    state = TermostatProcess::HEAT;
                    break;
                case TermostatProcess::END:
                    state = TermostatProcess::COOKING;
                    break;
                default:
                    break;
            }
            break;
        case PROCESS_NEXT_STATE:
            switch (state)
            {
                case TermostatProcess::STOP:
                    state = TermostatProcess::HEAT;
                    break;
                case TermostatProcess::HEAT:
                    state = TermostatProcess::COOKING;
                    break;
                case TermostatProcess::COOKING:
                    state = TermostatProcess::END;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    service();
}

time_t TermostatProcess::durationState(states st)
{
    if (state_begin[st] > 0)
        return now() - state_begin[st];
    else
        return 0;
}
