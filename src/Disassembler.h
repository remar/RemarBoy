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
  std::string formatByte(unsigned char val);
  std::string formatWord(unsigned short val);

  Memory *memory;
  std::map<unsigned char, std::string> opToMnemonic;
};

#endif
