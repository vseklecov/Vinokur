#ifndef __Buzzer_H__
#define __Buzzer_H__

#include <ProcessScheduler.h>

#include "pitches.h"

typedef struct
{
  int length;
  int *notes;
  int *beats;
} Song;

class Buzzer : public Process
{
public:
  Buzzer(Scheduler &manager, ProcPriority pr, unsigned int period, byte pin)
      : Process(manager, pr, period)
  {
    _pin = pin;
  }

  void setup(void);
  void service(void);

  void playSong(Song *);

private:
  byte _pin;
  unsigned long _delay;
  Song *_playSong;
  int _curNote;
};

// Ритм задается массивом из длительности нот и пауз между ними.
// "1" - четвертная нота, "2" - половинная, и т.д.
// Не забывайте, что пробелы должны быть тоже определенной длинны.
// "tempo" это скорость проигрывания мелодии.
// Для того, чтобы мелодия проигрывалась быстрее, вы
// должны уменьшить следующее значение.
// int twinki_notes[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// int twinki_beats[] = {      4,       8,       8,       4,       4, 4,       4,       4};
// int nokia_notes[] = {NOTE_E5, NOTE_D5, NOTE_F4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_D4,
//                      NOTE_E4, NOTE_B4, NOTE_A4, NOTE_C4, NOTE_E4, NOTE_A4};
// int nokia_beats[] = {8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 1};
// int chris_notes[] = {NOTE_C4, NOTE_A4, NOTE_A4, NOTE_G4,
//                      NOTE_A4, NOTE_F4, NOTE_C4, NOTE_C4,
//                      NOTE_C4, NOTE_A4, NOTE_A4, NOTE_AS4,
//                      NOTE_G4, NOTE_C5, NOTE_C5, NOTE_D4,
//                      NOTE_D4, NOTE_AS4,NOTE_AS4,NOTE_A4,
//                      NOTE_G4, NOTE_F4, NOTE_C4, NOTE_A4,
//                      NOTE_A4, NOTE_G4, NOTE_A4, NOTE_F4};
// int chris_beats[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,2};

#endif