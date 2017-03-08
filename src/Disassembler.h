#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <map>
#include "Memory.h"
#include "Instruction.h"

class Disassembler {
public:
  Disassembler(Memory *memory);
  Instruction disassemble(uint16_t address);

private:
  void setupOpToMnemonicMap();
  std::string formatByte(uint8_t val);
  std::string formatWord(uint16_t val);
  Instruction mkInstr(std::string mnemonic, uint16_t address);
  Instruction disassembleCB(uint16_t address);
  std::string getRegName(int reg);
  std::string getWideRegNameAF(int widereg);
  std::string getWideRegNameSP(int widereg);
  std::string getIndirectRegName(int indirectreg);

  Memory *memory;
  std::map<uint8_t, std::string> opToMnemonic;
};

#endif
