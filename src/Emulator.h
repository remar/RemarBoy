#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "Memory.h"
#include <string>
#include <list>

class Emulator {
public:
  Emulator(Memory *memory);
  void insertCart(std::string path);
  void step();
  void printState();

private:
  CPU *cpu;
  Memory *memory;
};

#endif
