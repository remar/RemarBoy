#include "DisassemblyEngine.h"
#include "Console.h"

DisassemblyEngine disassemblyEngine;

int main() {
  Console console(disassemblyEngine);

  disassemblyEngine.loadRom("/home/andreas/Spel/roms/gb/Tetris.gb");

  console.run();
}
