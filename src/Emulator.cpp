#include "Emulator.h"

Emulator::Emulator(Memory *memory) : memory(memory) {
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

void
Emulator::printState() {
  cpu->printState();
}
