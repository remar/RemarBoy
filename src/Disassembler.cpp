#include "Disassembler.h"

void
Disassembler::insertCart(std::string path) {
  emulator.insertCart(path);
}

void
Disassembler::step() {
  emulator.step();
}


#if 1
int main() {
  Disassembler disassembler;

  disassembler.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");

  for(int i = 0;i < 10;i++) {
    disassembler.step();
  }
}
#endif
