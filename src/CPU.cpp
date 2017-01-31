#include "CPU.h"

CPU::CPU(const Memory* memory) : memory(memory) {
  PC = 0x100; // Program starts at 0x100
  SP = 0xfffe;
}

void
CPU::step() {

}
