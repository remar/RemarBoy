#include "Emulator.h"

Emulator::Emulator() {
  memory = new Memory();
  cpu = new CPU(memory);
}

void
Emulator::insertCart(std::string path) {
  memory->insertCart(path);
}

#if 0
int main() {
  Emulator emulator;
  emulator.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");
}
#endif
