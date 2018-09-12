#include <Arduino.h>
#include <EEPROM.h>

#include "Fraction.h"
#include "Functions.h"
#include "Keys.h"

FractionScreen::FractionScreen(UTFT &_lcd, FractionProcess &_fp)
    :lcd(_lcd), fp(_fp)
{
    x = lcd.getFontXsize();
    y = lcd.getFontYsize();
}

/* Шрифт Tahoma1251 24х32 экран 320x480
  01234567890123456789
  --------------------
0|  Дробный перегон   |
1|Нагрев 00:00:00     |
2| Куб 000.0  00.0%   |
3| Деф 000.0  00.0%   |
4|                    |
5|                    |
6|                    |
7|                    |
8|                    |
9|000.0   00:00  000мм|
  --------------------
  01234567890123456789
*/
void FractionScreen::draw()
{
    lcd.clrScr();
    lcd.setColor(VGA_AQUA);
    lcd.print((char*)"Дробный перегон", CENTER, 0);

    lcd.print((char*)"Куб", x, 2*y);
    lcd.printChar('%', 16*x, 2*y);

    lcd.print((char*)"Деф", x, 3*y);
    lcd.printChar('%', 16*x, 3*y);

    lcd.print((char*)"мм", 18*x, 9*y);
}

void FractionScreen::update()
{
    lcd.setColor(VGA_AQUA);
    lcd.print(fp.getStateString(), 0, y);
    lcd.print(formatTime(2, fp.durationState()), 7*x, y);

    lcd.printNumF(fp.sp.getThempKub(), 1, 5*x, 2*y, '.', 5);
    lcd.printNumF(alcLiq(fp.sp.getThempKub()), 1, 12*x, 2*y, '.', 4);

    lcd.printNumF(fp.sp.getThempDef(), 1, 5*x, 3*y, '.', 5);
    lcd.printNumF(alcSteam(fp.sp.getThempDef()), 1, 12*x, 3*y, '.', 4);

    if (fp.tempBoiling > 0)
        lcd.printNumF(fp.tempBoiling, 1, 0, 9*y, '.', 5);
    lcd.print(formatTime(3, 0), CENTER, 9*y);
    lcd.printNumI(round(fp.sp.getPressure() * 7.5E-3), 15*x, 9*y, 3);
}


FractionProcess::FractionProcess(Scheduler &manager, ProcPriority pr, unsigned int period, SensorsProcess &_sp)
    : Process(manager, pr, period), sp(_sp)
{
    for (int i=0; i<8; i++)
        state_begin[i] = 0;
    tempBoiling = 0;
}

void FractionProcess::setup()
{
    state_begin[STATE_STOP] = now();
}

void FractionProcess::service()
{
    switch(state)
    {
        case STATE_STOP:
            setHeating(0);
            setCooling(0);
            if ((sp.getThempKub() - sp.getThempDef()) > 5)
                state = STATE_HEAT;
            break;
        case STATE_HEAT:
            setHeating(100);
            if (sp.getThempKub() > tempKubCoolOn || sp.getThempDef() > tempDefCoolOn)
                setCooling(100);
            else if (sp.getThempKub() < (tempKubCoolOn-5) && sp.getThempDef() < (tempDefCoolOn-5))
            {
                setCooling(0);
                tempBoiling = 0;
            }
            if (tempBoiling > 0)
            {
                state = STATE_EQUIL;
                pause = equilTime * 1000 + millis();
            } else if (getCooling())
            {
                if (tempKub == 0)
                {
                    tempKub = sp.getThempKub();
                    startBoiling = millis();
                }
                if ((millis()-startBoiling)>20000)
                {
                    if (abs(sp.getThempKub()-tempKub)<0.1)
                    {
                        tempBoiling = sp.getThempKub();
                        startBoiling = millis();
                    }
                }
            }
            break;
        case STATE_EQUIL:
            setHeating(40);
            setCooling(100);
            if (pause == 0)
                pause = equilTime * 1000 + millis();
            if (millis() > pause)
            {
                pause = 0;
                state = STATE_HEAD;
            }
            break;
        case STATE_HEAD:
            setHeating(40);
            setCooling(100);
            break;
        case STATE_HEART:
            setHeating(80);
            setCooling(100);
            if (sp.getThempKub() > tempKubHeartOff || sp.getThempDef() > tempDefHeartOff)
                state = STATE_TAILS;
            break;
        case STATE_TAILS:
            setHeating(100);
            setCooling(100);
            if (sp.getThempKub() > tempKubDistOff || sp.getThempDef() > tempDefDistOff)
                state = STATE_END;
            break;
        case STATE_END:
            setHeating(0);
            if (pause == 0)
                pause = millis() + 20000;
            if (millis() > pause)
            {
                setCooling(0);
                disable();
            }
            break;
        case STATE_CANCEL:
            setHeating(0);
            setCooling(0);
            disable();
            break;
        default:
            break;
    }
}

void FractionProcess::recive(char ch)
{
    switch (ch)
    {
        case PROCESS_CANCEL:
            if (state == STATE_CANCEL)
                disable();
            else
                state = STATE_CANCEL;
            break;
        case PROCESS_START:
            if (state == STATE_STOP || state == STATE_CANCEL || state == STATE_END)
                state = STATE_HEAT;
            break;
        case PROCESS_PREV_STATE:
            if (state != STATE_STOP)
                state = (FractionProcess::states)(int(state)-1);
            break;
        case PROCESS_NEXT_STATE:
            if (state != STATE_END && state != STATE_CANCEL)
                state = (FractionProcess::states)(int(state)+1);
            break;
        default:
            break;
    }
    service();
}

time_t FractionProcess::durationState(states st)
{
    if (state_begin[st] > 0)
        return now() - state_begin[st];
    else
        return 0;
}

void FractionProcess::readDataEEPROM()
{
    //if (EEPROM[0] == 255) // EEPROM чистый записать параметры прегонки браги
    //    DistillationProcess::readDataEEPROM();
    if (EEPROM[0] == 1)
    { // EEPROM с параметрами для перегонки браги, записать параметры для дробной перегонки
        tempKubCoolOn   = 75.0;
        tempDefCoolOn   = 50.0;
        tempKubDistOff  = 98.0; // 8%
        tempDefDistOff  = 99.1; // 10%
        equilTime       = 1200; // 20 минут
        tempKubHeartOff = 92.0;
        tempDefHeartOff = 89.0;
        writeDataEEPROM();
    } else
    {
        int temp[7];
        EEPROM.get(10, temp); // Второй блок параметров
        tempKubCoolOn = temp[0] / 10;
        tempDefCoolOn = temp[1] / 10;
        tempKubDistOff = temp[2] / 10;
        tempDefDistOff = temp[3] / 10;
        equilTime = temp[4];
        tempKubHeartOff = temp[5] / 10;
        tempDefHeartOff = temp[6] / 10;
    }
}

void FractionProcess::writeDataEEPROM()
{
    int temp[7];
    byte hi, lo;
    temp[0] = int(tempKubCoolOn * 10);
    temp[1] = int(tempDefCoolOn * 10);
    temp[2] = int(tempKubDistOff * 10);
    temp[3] = int(tempDefCoolOn * 10);
    temp[4] = equilTime;
    temp[5] = int(tempKubHeartOff * 10);
    temp[6] = int(tempDefHeartOff * 10);
    if (EEPROM[0] == 255) // EEPROM чистый ОШИБКА!
        return;
    if (EEPROM[0] == 1)
    {
        EEPROM.write(0, 2);
        EEPROM.put(10, temp);
    }
    else
    {
        for (int i = 0; i < 7; i++)
        {
            hi = highByte(temp[i]);
            lo = lowByte(temp[i]);
            EEPROM.update(10 + i * 2, lo);
            EEPROM.update(11 + i * 2, hi);
        }
    }
}
