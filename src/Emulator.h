#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "Memory.h"
#include <string>
#include <list>

class Emulator {
public:
  Emulator();
  void insertCart(std::string path);
  void step();
  std::list<int> getVisited();

private:
  CPU *cpu;
  Memory *memory;
};

#endif
