#ifndef CPU_H
#define CPU_H

#include "Memory.h"

const int last_use_long = 0;
const int last_use_short = 1;

class CPU {
private:
  const Memory& memory;

  unsigned short PC;
  unsigned short SP;

  unsigned short AF;
  unsigned short BC;
  unsigned short DE;
  unsigned short HL;

  unsigned char A, F;
  unsigned char B, C;
  unsigned char D, E;
  unsigned char H, L;

  int AF_last_use;
  int BC_last_use;
  int DE_last_use;
  int HL_last_use;

public:
  CPU(const Memory& memory);
  void step();
};

#endif
