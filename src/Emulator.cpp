#include "Emulator.h"

Emulator::Emulator() {
  memory = new Memory();
  cpu = new CPU(memory);
}

void
Emulator::insertCart(std::string path) {
  memory->insertCart(path);
}

void
Emulator::step() {
  cpu->step();
}

#if 1
int main() {
  Emulator emulator;

  emulator.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");

  for(int i = 0;i < 10;i++) {
    emulator.step();
  }
}
#endif
