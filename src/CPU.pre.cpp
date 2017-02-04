#include "CPU.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

const unsigned char ZF = 0x80; // Zero flag
const unsigned char NF = 0x40; // Negative op flag
const unsigned char HF = 0x20; // Half-carry flag
const unsigned char CF = 0x10; // Carry flag

CPU::CPU(Memory* memory) : mem(memory) {
  PC = 0x100; // Program starts at 0x100
  SP = 0xfffe;
}

void
CPU::step() {
  unsigned char op = mem->getOp(PC++);
  switch(op) {
  case 0x00: // NOP
    mem->cycles = 1;
    break;

  case 0xC3: // JP nn
    PC = mem->getWord(PC);
    mem->cycles = 4;
    break;

// --------- BEGIN GENERATED CODE ---------
// --------- END GENERATED CODE ---------

  default:
    std::stringstream fmt;
    fmt << "OP not implemented: 0x" << std::hex << std::uppercase << (int)op;
    throw std::out_of_range(fmt.str());
  }
}
