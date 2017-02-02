#include "CPU.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

CPU::CPU(Memory* memory) : memory(memory) {
  PC = 0x100; // Program starts at 0x100
  SP = 0xfffe;
}

void
CPU::step() {
  unsigned char op = memory->getByte(PC++);
  switch(op) {
  case 0x00: // NOP
    std::cout << "NOP" << '\n';
    break;

  default:
    std::stringstream fmt;
    fmt << "OP not implemented: 0x" << std::hex << std::uppercase << (int)op;
    throw std::out_of_range(fmt.str());
  }
}
