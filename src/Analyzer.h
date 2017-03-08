#ifndef DISASSEMBLY_ENGINE_H
#define DISASSEMBLY_ENGINE_H

#include "Memory.h"
#include "Emulator.h"
#include "Disassembler.h"

class Analyzer {
public:
  Analyzer();
  void loadRom(std::string path);
  void runSteps(int steps);
  void printDisassembly();
  int getCyclesPerSecond();

private:
  Memory memory;
  Emulator *emulator;
  Disassembler *disassembler;
  int cyclesPerSecondLastRun;
};

#endif