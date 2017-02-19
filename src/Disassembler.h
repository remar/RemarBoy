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
  Instruction mkInstr(std::string mnemonic, unsigned short address);
  std::string getRegName(int reg);
  std::string getWideRegNameAF(int widereg);
  std::string getWideRegNameSP(int widereg);
  std::string getIndirectRegName(int indirectreg);

  Memory *memory;
  std::map<unsigned char, std::string> opToMnemonic;
};

#endif
