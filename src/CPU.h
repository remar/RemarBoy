#ifndef CPU_H
#define CPU_H

#include "Memory.h"

class CPU {
public:
  CPU(Memory* memory);
  void step();
  void printState();

private:
  void doCB();
  void printByteReg(std::string name, unsigned char value);
  void printWordReg(std::string name, unsigned short value);
  void printRegPair(std::string name1, unsigned char value1,
		    std::string name2, unsigned char value2);

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
