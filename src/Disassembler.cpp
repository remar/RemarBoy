#include "Disassembler.h"

#include <iostream>

Disassembler::Disassembler(Memory *memory) : memory(memory) {}

std::string
Disassembler::disassemble(unsigned short address) {
  unsigned char op = memory->getByte(address);
  switch(op) {
  case 0x00:
    return "NOP";
  default:
    return "---";
  }
}
