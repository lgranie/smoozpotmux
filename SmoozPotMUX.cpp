/*
  SmoozPotMUX.cpp - Library for 4051 multiplexer
 Created by Laurent Granié
 (Inspired by code from e-licktronic.com)
 Released into the public domain.
 */

#include "Arduino.h"
#include "SmoozPotMUX.h"

SmoozPotMUX SmoozPot;

void SmoozPotMUX::Initialize()
{
  // Init Potentiometres
  pinMode(8, OUTPUT);    // clock Pin du compteur CD4520
  pinMode(12, INPUT);    // Pin temoin d'initialisation du compteur binaire CD4520

  //Reset to 0 du compteur binaire CD4520
  do{ 
    PORTB &=0;//digitalWrite(8,LOW);
    PORTB |=1;//digitalWrite(8,HIGH);
  }
  while (digitalRead(12)==HIGH);

  do{
    PORTB |=1;//digitalWrite(8,HIGH);
    PORTB &=0;//digitalWrite(8,LOW);
  }
  while (digitalRead(12)==LOW);

  pinMode(12, OUTPUT);// Conflits with SRIO lib

  indx = 0;
  pot_val     = (byte*) malloc(NB_BOARD * 8); // 8 knobs per board
  smooz_val   = (uint16_t*) malloc(NB_BOARD * 8 * sizeof(uint16_t));

  // read the first line of knobs
  for(register byte _pot = 0; _pot < NB_BOARD * 8; _pot++) {
    PORTB &=0;//digitalWrite(8,LOW);
    pot_val[_pot] = (analogRead(_pot >> 3) >> 2);//convert 10bits 0->1024 to 8bits 0->255
    PORTB |=1;//digitalWrite(8,HIGH);
  }
}

boolean SmoozPotMUX::Update() {
  // read the first line of knobs
  for(register byte _pot = 0; _pot < NB_BOARD * 8; _pot++) {
    PORTB &=0;//digitalWrite(8,LOW);
    if(indx > 0) {
      const byte _analog = (analogRead(_pot >> 3) >> 2);//convert 10bits 0->1024 to 8bits 0->255

      // Anti bagotement
      (abs(pot_val[_pot] - _analog) > 32) 
        ? smooz_val[_pot] += smooz_val[_pot] 
        : smooz_val[_pot] += _analog;
    } 
    else {
      smooz_val[_pot] = 0;
    }
    PORTB |=1;//digitalWrite(8,HIGH);
  }

  // Compute pot_val[]
  if(indx == AVG_SIZE) {
    byte _pot_val;
    boolean _chg = false;
    for(register byte _pot = 0; _pot < NB_BOARD * 8; _pot++) {
      // Compute pot value
      _pot_val = byte(smooz_val[_pot] / AVG_SIZE);
      //_pot_val = map(_pot_val, 6, 247, 0, 255);
      _pot_val = max(_pot_val, 0);
      _pot_val = constrain(_pot_val, 0, 255);

      // re anti bagotement
      if (abs(pot_val[_pot] - _pot_val) > 0) {
        pot_val[_pot] = _pot_val;
        _chg = true;
      }
    }
    indx = 0;
    return _chg;
  }

  indx++;
  return false;
}

byte SmoozPotMUX::Read(const byte _pot) { 
  return pot_val[_pot];
}

void SmoozPotMUX::Write(const byte _pot, const byte _pot_val) {
  pot_val[_pot] = _pot_val;
}

void SmoozPotMUX::Reset() {
  indx = 0;
}
