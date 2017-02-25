#ifndef DISASSEMBLY_ENGINE_H
#define DISASSEMBLY_ENGINE_H

#include "Memory.h"
#include "Emulator.h"
#include "Disassembler.h"

class DisassemblyEngine {
public:
  DisassemblyEngine();
  void loadRom(std::string path);
  void runSteps(int steps);
  void printDisassembly();

private:
  Memory memory;
  Emulator *emulator;
  Disassembler *disassembler;
};

#endif
