#ifndef DISASSEMBLY_ENGINE_H
#define DISASSEMBLY_ENGINE_H

#include "Memory.h"
#include "Disassembler.h"
#include "CPU.h"
#include "Input.h"
#include "LCD.h"

class Analyzer {
public:
  Analyzer();
  void loadRom(std::string path);
  void runSteps(int steps);
  void printDisassembly();
  int getCyclesPerSecond();

private:
  void step();

  Memory memory;
  CPU *cpu;
  LCD *lcd;
  Input *input;
  Disassembler *disassembler;
  int cyclesPerSecondLastRun;
};

#endif
