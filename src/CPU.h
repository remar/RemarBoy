#ifndef CPU_H
#define CPU_H

#include "Memory.h"

class CPU {
private:
  const Memory* memory;

  unsigned short PC;
  unsigned short SP;

  unsigned char A, F;
  unsigned char B, C;
  unsigned char D, E;
  unsigned char H, L;

public:
  CPU(const Memory* memory);
  void step();
};

#endif
