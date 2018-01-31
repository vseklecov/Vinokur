#ifndef __Buzzer_H__
#define __Buzzer_H__

#include <ProcessScheduler.h>

#include "pitches.h"

typedef struct {
  int length;
  int *notes;
  int *beats;
} Song;

Song Twinki;
Song Nokia;
Song Chris;
Song S;

class Buzzer: public Process
{
  public:
    Buzzer(Scheduler &manager, ProcPriority pr, unsigned int period, byte pin)
      : Process(manager, pr, period)
      {
        _pin = pin; 
      }

    void setup(void);
    void service(void);
    
    void playSong(Song*);

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
int s_notes[] = {NOTE_C4, NOTE_G3};
int s_beats[] = {    250,     125};

int twinki_notes[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int twinki_beats[] = {      4,       8,       8,       4,       4, 4,       4,       4};

int nokia_notes[] = {NOTE_E5, NOTE_D5, NOTE_F4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_D4,
                     NOTE_E4, NOTE_B4, NOTE_A4, NOTE_C4, NOTE_E4, NOTE_A4};
int nokia_beats[] = {8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 1};

int chris_notes[] = {NOTE_C4, NOTE_A4, NOTE_A4, NOTE_G4,
                     NOTE_A4, NOTE_F4, NOTE_C4, NOTE_C4,
                     NOTE_C4, NOTE_A4, NOTE_A4, NOTE_AS4,
                     NOTE_G4, NOTE_C5, NOTE_C5, NOTE_D4,
                     NOTE_D4, NOTE_AS4,NOTE_AS4,NOTE_A4,
                     NOTE_G4, NOTE_F4, NOTE_C4, NOTE_A4,
                     NOTE_A4, NOTE_G4, NOTE_A4, NOTE_F4};
int chris_beats[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,4,2};

void Buzzer::setup() {
  pinMode(_pin, OUTPUT);
  Twinki.length = 8;
  Twinki.notes = twinki_notes;
  Twinki.beats = twinki_beats;
  Nokia.length = 13;
  Nokia.notes = nokia_notes;
  Nokia.beats = nokia_beats;
  Chris.length = 28;
  Chris.notes = chris_notes;
  Chris.beats = chris_beats;
  S.length = 1;
  S.notes = s_notes;
  S.beats = s_beats;
  
  for (int i; i < Twinki.length; i++) {
    Twinki.beats[i]=1000/Twinki.beats[i];  
  }
  for (int i; i < Nokia.length; i++) {
    Nokia.beats[i]=1000/Nokia.beats[i];  
  }
  for (int i; i < Chris.length; i++) {
    Chris.beats[i]=1000/Chris.beats[i];  
  }
  //playSong(Nokia);
}


void Buzzer::playSong(Song *_song) 
{
  if (_playSong==NULL) {
    _playSong = _song;
    _curNote = 0;
    _delay = 0;
    enable();
  }
//  for (int i = 0; i < _song.length; i++) {
//    if (_song.notes[i] == 0) {
//      delay(_song.beats[i]);
//    } else {
//      tone(_pin, _song.notes[i], _song.beats[i]);
//      delay(_song.beats[i]*1.3);
//    }
//  }
//  noTone(_pin);
}


void Buzzer::service() {
  if (_delay < millis()) {
    if (_playSong!=NULL) {
      if (_curNote<_playSong->length) {
        if (_playSong->notes[_curNote]==0) {
          _delay=millis()+_playSong->beats[_curNote];
        } else {
          tone(_pin, _playSong->notes[_curNote], _playSong->beats[_curNote]);
          _delay=millis()+_playSong->beats[_curNote]*1.3;
        }
        _curNote++;
      } else {
        noTone(_pin);
        _playSong=NULL;
        _curNote=0;
        disable();
      }
    }
  }
}

#endif
