#ifndef SmoozPotMUX_h     
#define SmoozPotMUX_h
#include "Arduino.h"

// Nb licklogic board
#define NB_BOARD 1

// Smooz reading
#define AVG_SIZE 16

class SmoozPotMUX
{
public:
  void Initialize();
  boolean Update();
  byte Read(byte pin);
  void Write(byte pin, byte value);
  void Reset();
private:
  byte  indx;
  byte* pot_val;

  uint16_t* smooz_val;

};
  
extern SmoozPotMUX SmoozPot;


#endif // Fin si
