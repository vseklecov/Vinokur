#include <Arduino.h>

#include "Buzzer.h"

Song S;

int s_notes[] = {NOTE_C4, NOTE_G3};
int s_beats[] = {    250,     125};

void Buzzer::setup() {
  pinMode(_pin, OUTPUT);
  S.length = 1;
  S.notes = s_notes;
  S.beats = s_beats;

  // Twinki.length = 8;
  // Twinki.notes = twinki_notes;
  // Twinki.beats = twinki_beats;
  // Nokia.length = 13;
  // Nokia.notes = nokia_notes;
  // Nokia.beats = nokia_beats;
  // Chris.length = 28;
  // Chris.notes = chris_notes;
  // Chris.beats = chris_beats;
  // for (int i; i < Twinki.length; i++) {
  //   Twinki.beats[i]=1000/Twinki.beats[i];  
  // }
  // for (int i; i < Nokia.length; i++) {
  //   Nokia.beats[i]=1000/Nokia.beats[i];  
  // }
  // for (int i; i < Chris.length; i++) {
  //   Chris.beats[i]=1000/Chris.beats[i];  
  // }
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