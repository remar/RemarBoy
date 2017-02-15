#include "Emulator.h"
#include "Disassembler.h"
#include "Memory.h"
#include <iostream>
#include <iomanip>

Memory memory;

Emulator emulator(&memory);
Disassembler disasm(&memory);

void printDisassembly() {
  std::list<int> visited = memory.getVisited();
  std::list<int>::iterator it = visited.begin();
  for(;it != visited.end();it++) {
    std::cout << "[" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << *it << "] " << disasm.disassemble(*it).asString() << std::endl;
  }
}

int main() {
  memory.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");
  emulator.printState();
  for(int i = 0;i < 100000;i++) {
    emulator.step();
  }

  emulator.printState();

  printDisassembly();
}
