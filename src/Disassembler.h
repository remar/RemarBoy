#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <map>
#include "Memory.h"

class Disassembler {
public:
  Disassembler(Memory *memory);
  std::string disassemble(unsigned short address);

private:
  void setupOpToMnemonicMap();
  std::string format(unsigned short val, int width);

  Memory *memory;
  std::map<unsigned char, std::string> opToMnemonic;
};

#endif
