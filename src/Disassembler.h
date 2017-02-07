#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include "Memory.h"

class Disassembler {
public:
  Disassembler(Memory *memory);
  std::string disassemble(unsigned short address);

private:
  Memory *memory;
};

#endif
