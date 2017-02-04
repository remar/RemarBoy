#include "Disassembler.h"

#include <iostream>

void
Disassembler::insertCart(std::string path) {
  emulator.insertCart(path);
}

void
Disassembler::step() {
  emulator.step();
}

void
Disassembler::disassembleVisited() {
  std::list<int> visited = emulator.getVisited();

  std::list<int>::iterator it = visited.begin();
  for(;it != visited.end();it++) {
    std::cout << std::hex << *it << std::endl;
  }
}

#if 1
int main() {
  Disassembler disassembler;

  disassembler.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");

  for(int i = 0;i < 10000;i++) {
    disassembler.step();
  }

  disassembler.disassembleVisited();
}
#endif
