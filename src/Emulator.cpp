#include "Emulator.h"

Emulator::Emulator(Memory *memory) : memory(memory) {
  cpu = new CPU(memory);
  lcd = new LCD(memory);
  input = new Input(memory);
}

void
Emulator::insertCart(std::string path) {
  memory->insertCart(path);
}

void
Emulator::step() {
  cpu->step();
  lcd->step();
  input->step();
}

void
Emulator::printState() {
  cpu->printState();
}
