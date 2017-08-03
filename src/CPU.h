#ifndef CPU_H
#define CPU_H

#include "Memory.h"

class CPU {
public:
  CPU(Memory* memory);
  void reset();
  void step();
  void printState();
  int getTotalCycles();
  void set(std::string name, uint16_t val);
  uint16_t get(std::string name);

private:
  void doCB();
  void printWordReg(std::string name, uint16_t value);

  int accum_cycles;

  Memory* mem;

  uint16_t PC;
  uint16_t SP;

  union reg_pair {
    uint16_t word;
    struct {
#ifdef BIG_ENDIAN
      uint8_t low, high;
#else
      uint8_t high, low;
#endif
    };
  };

  reg_pair AF, BC, DE, HL;

  bool IME; // Interrupt Master Enable
};

#endif
