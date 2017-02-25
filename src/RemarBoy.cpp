#include "DisassemblyEngine.h"

DisassemblyEngine disassemblyEngine;

int main() {
  disassemblyEngine.loadRom("/home/andreas/Spel/roms/gb/Tetris.gb");
  disassemblyEngine.runSteps(100000);
  disassemblyEngine.printDisassembly();
}
