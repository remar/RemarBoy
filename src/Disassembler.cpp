#include "Disassembler.h"

#include <iostream>
#include <sstream>
#include <iomanip>

Disassembler::Disassembler(Memory *memory) : memory(memory) {
  setupOpToMnemonicMap();
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x x . . . . . . . . . . . . . . 0
// 1 . x . . . . . . x . . . . . . . 1
// 2 x x . . . . . . x . . . . . . x 2
// 3 x x . . . . . . x . . . . . . . 3
// 4 . . . . . . . . . . . . . . . . 4
// 5 . . . . . . . . . . . . . . . . 5
// 6 . . . . . . . . . . . . . . . . 6
// 7 . . . . . . . . . . . . . . . . 7
// 8 . . . . . . . . . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A . . . . . . . . . . . . . . . . A
// B . . . . . . . . . . . . . . . . B
// C x . . x . . . . x x x . . x . . C
// D . . . . . . . . . x . . . . . . D
// E x . x . . . x . . x . . . . . . E
// F x . . x . . . . . . x x . . x . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

Instruction
Disassembler::disassemble(unsigned short address) {
  unsigned char op = memory->getByte(address);

  if((op & 0xe7) == 0x20) { // JR conditional,n
    std::string neg = (op & 0x08) == 0x08 ? "":"N";
    std::string flag = (op & 0x10) == 0x10 ? "C":"Z";
    unsigned char n = memory->getByte(address + 1);
    return Instruction("JR "+neg+flag+",0x"+formatByte(n),
		       2,
		       memory->getBytes(address));
  } else if((op & 0xcf) == 0x01) { // LD rr,nn
    int nn = memory->getWord(address + 1);
    return Instruction("LD " + getWideRegNameSP((op & 0x30) >> 4) + ",0x"+formatWord(nn),
		       3,
		       memory->getBytes(address));
  } else {
    std::string mnemonic = opToMnemonic[op];
    if(mnemonic != "") {
      int pos;
      if((pos = mnemonic.find("nn")) != std::string::npos) {
	return Instruction(mnemonic.replace(pos, 2, formatWord(memory->getWord(address+1))),
			   3,
			   memory->getBytes(address));
      } else if((pos = mnemonic.find("n")) != std::string::npos) {
	return Instruction(mnemonic.replace(pos, 1, formatByte(memory->getByte(address+1))),
			   2,
			   memory->getBytes(address));
      }
      return Instruction(mnemonic, 1, memory->getBytes(address));
    }

    return Instruction("---", 0, 0);
  }
}

void
Disassembler::setupOpToMnemonicMap() {
  opToMnemonic[0x00] = "NOP";
  opToMnemonic[0x18] = "JR 0xn";
  opToMnemonic[0x2F] = "CPL";
  opToMnemonic[0xC0] = "RET NZ";
  opToMnemonic[0xC3] = "JP 0xnn";
  opToMnemonic[0xC8] = "RET Z";
  opToMnemonic[0xC9] = "RET";
  opToMnemonic[0xCA] = "JP Z,0xnn";
  opToMnemonic[0xCD] = "CALL 0xnn";
  opToMnemonic[0xD9] = "RETI";
  opToMnemonic[0xE0] = "LD (0xFFn),A";
  opToMnemonic[0xE2] = "LD (0xFF00 + C),A";
  opToMnemonic[0xE6] = "AND 0xn";
  opToMnemonic[0xE9] = "JP (HL)";
  opToMnemonic[0xEA] = "LD (0xnn),A";
  opToMnemonic[0xF0] = "LD A,(0xFFn)";
  opToMnemonic[0xF3] = "DI";
  opToMnemonic[0xFA] = "LD A,(0xnn)";
  opToMnemonic[0xFB] = "EI";
  opToMnemonic[0xFE] = "CP 0xn";
}

std::string
Disassembler::formatByte(unsigned char val) {
  std::stringstream fmt;
  fmt << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << (int)val;
  return fmt.str();
}

std::string
Disassembler::formatWord(unsigned short val) {
  std::stringstream fmt;
  fmt << std::hex << std::setfill('0') << std::uppercase << std::setw(4) << val;
  return fmt.str();
}

std::string
Disassembler::getWideRegNameSP(int widereg) {
  const std::string wideregs[] = {"BC", "DE", "HL", "SP"};
  return wideregs[widereg];
}
