#ifndef CPU_H
#define CPU_H

#include "Memory.h"

class CPU {
private:
  Memory* mem;

  unsigned short PC;
  unsigned short SP;

  unsigned char A, F;
  unsigned char B, C;
  unsigned char D, E;
  unsigned char H, L;

public:
  CPU(Memory* memory);
  void step();
};

#endif
