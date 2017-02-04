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

std::list<int>
Emulator::getVisited() {
  return memory->getVisited();
}
