#ifndef CPU_H
#define CPU_H

#include "Memory.h"

class CPU {
public:
  CPU(Memory* memory);
  void step();
  void printState();
  int getTotalCycles();

private:
  void doCB();
  void printWordReg(std::string name, unsigned short value);

  int accum_cycles;

  Memory* mem;

  unsigned short PC;
  unsigned short SP;

  union reg_pair {
    unsigned short word;
    struct {
#ifdef BIG_ENDIAN
      unsigned char low, high;
#else
      unsigned char high, low;
#endif
    };
  };

  reg_pair AF, BC, DE, HL;

  bool IME; // Interrupt Master Enable
};

#endif
