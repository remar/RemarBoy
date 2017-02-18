#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <map>
#include "Memory.h"
#include "Instruction.h"

class Disassembler {
public:
  Disassembler(Memory *memory);
  Instruction disassemble(unsigned short address);

private:
  void setupOpToMnemonicMap();
  std::string formatByte(unsigned char val);
  std::string formatWord(unsigned short val);
  std::string getWideRegNameSP(int widereg);

  Memory *memory;
  std::map<unsigned char, std::string> opToMnemonic;
};

#endif
