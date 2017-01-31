#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "Memory.h"
#include <string>

class Emulator {
public:
  Emulator();
  void insertCart(std::string path);

private:
  CPU *cpu;
  Memory *memory;
};

#endif
