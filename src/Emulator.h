#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "LCD.h"
#include "Input.h"
#include "Memory.h"
#include <string>
#include <list>

class Emulator {
public:
  Emulator(Memory *memory);
  void insertCart(std::string path);
  void step();
  void printState();
  int getTotalCPUCycles();

private:
  CPU *cpu;
  LCD *lcd;
  Input *input;
  Memory *memory;
};

#endif
