#include "Disassembler.h"

#include <iostream>
#include <sstream>
#include <iomanip>

Disassembler::Disassembler(Memory *memory) : memory(memory) {
  setupOpToMnemonicMap();
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x x x x x x x . . x x x x x x . 0
// 1 . x x x x x x . x x x x x x x . 1
// 2 x x x x x x x . x x x x x x x x 2
// 3 x x x x x x x . x x x x x x x . 3
// 4 x x x x x x x x x x x x x x x x 4
// 5 x x x x x x x x x x x x x x x x 5
// 6 x x x x x x x x x x x x x x x x 6
// 7 x x x x x x . x x x x x x x x x 7
// 8 x x x x x x x x x x x x x x x x 8
// 9 x x x x x x x x x x x x x x x x 9
// A x x x x x x x x x x x x x x x x A
// B x x x x x x x x x x x x x x x x B
// C x x . x . x . x x x x . . x . x C
// D . x . . . x . x . x . . . . . x D
// E x x x . . x x x . x . . . . . x E
// F x x . x . x . x . . x x . . x x F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

Instruction
Disassembler::disassemble(unsigned short address) {
  unsigned char op = memory->getByte(address);

  if((op & 0xe7) == 0x20) { // JR conditional,n
    std::string neg = (op & 0x08) == 0x08 ? "":"N";
    std::string flag = (op & 0x10) == 0x10 ? "C":"Z";
    unsigned char n = memory->getByte(address + 1);
    return mkInstr("JR "+neg+flag+",0x"+formatByte(n), address);
  } else if((op & 0xcf) == 0x01) { // LD rr,nn
    int nn = memory->getWord(address + 1);
    return mkInstr("LD " + getWideRegNameSP((op & 0x30) >> 4) + ",0x"+formatWord(nn), address);
  } else if((op & 0xcf) == 0x02) { // LD (rr+-),A
    return mkInstr("LD " + getIndirectRegName((op & 0x30) >> 4) + ",A", address);
  } else if((op & 0xcf) == 0x03) { // INC rr
    return mkInstr("INC " + getWideRegNameSP((op & 0x30) >> 4), address);
  } else if((op & 0xc7) == 0x04) { // INC r
    return mkInstr("INC " + getRegName((op & 0x38) >> 3), address);
  } else if((op & 0xc7) == 0x05) { // DEC r
    return mkInstr("DEC " + getRegName((op & 0x38) >> 3), address);
  } else if((op & 0xc7) == 0x06) { // LD r,n
    unsigned char n = memory->getByte(address + 1);
    return mkInstr("LD " + getRegName((op & 0x38) >> 3) + ",0x" + formatByte(n), address);
  } else if((op & 0xcf) == 0x09) { // ADD HL,rr
    return mkInstr("ADD HL," + getWideRegNameSP((op & 0x30) >> 4), address);
  } else if((op & 0xcf) == 0x0a) { // LD A,(rr+-)
    return mkInstr("LD A," + getIndirectRegName((op & 0x30) >> 4), address);
  } else if((op & 0xcf) == 0x0b) { // DEC rr
    return mkInstr("DEC " + getWideRegNameSP((op & 0x30) >> 4), address);
  } else if((op & 0xc0) == 0x40 && op != 0x76) { // LD r,r', LD (HL),r, and LD r,(HL)
    std::string target = getRegName((op & 0x38) >> 3);
    std::string source = getRegName(op & 0x07);
    return mkInstr("LD "+target+","+source, address);
  } else if(op >= 0x80 && op <= 0xbf) { // ADD, ADC, SUB, SBC, AND, XOR, OR, CP
    const std::string mne[] = {"ADD","ADC","SUB","SBC","AND","XOR","OR","CP"};
    return mkInstr(mne[(op - 0x80) >> 3] + " " + getRegName(op & 0x07), address);
  } else if((op & 0xcf) == 0xc1) { // POP rr
    return mkInstr("POP " + getWideRegNameAF((op & 0x30) >> 4), address);
  } else if((op & 0xcf) == 0xc5) { // PUSH rr
    return mkInstr("PUSH " + getWideRegNameAF((op & 0x30) >> 4), address);
  } else if((op & 0xc7) == 0xc7) { // RST n
    int dest = ((op & 0x28) >> 3);
    return mkInstr("RST " + std::to_string(dest) + " (0x" + formatByte(dest * 8) + ")", address);
  } else if(op == 0xcb) { // CB prefix
    return disassembleCB(address);
  } else {
    std::string mnemonic = opToMnemonic[op];
    if(mnemonic != "") {
      int pos;
      if((pos = mnemonic.find("nn")) != std::string::npos) {
	return mkInstr(mnemonic.replace(pos, 2, formatWord(memory->getWord(address+1))), address);
      } else if((pos = mnemonic.find("n")) != std::string::npos) {
	return mkInstr(mnemonic.replace(pos, 1, formatByte(memory->getByte(address+1))), address);
      }
      return mkInstr(mnemonic, address);
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

Instruction
Disassembler::mkInstr(std::string mnemonic, unsigned short address) {
  const int lengths[] = {
  //0 1 2 3 4 5 6 7 8 9 A B C D E F
    1,3,1,1,1,1,2,1,3,1,1,1,1,1,2,1, // 0
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1, // 1
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1, // 2
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1, // 3
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 4
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 5
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 6
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 7
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 8
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 9
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // A
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // B
    1,1,3,3,3,1,2,1,1,1,3,2,3,3,2,1, // C
    1,1,3,0,3,1,2,1,1,1,3,0,3,0,2,1, // D
    2,1,2,0,0,1,2,1,2,1,3,0,0,0,2,1, // E
    2,1,2,1,0,1,2,1,2,1,3,1,0,0,2,1  // F
  //0 1 2 3 4 5 6 7 8 9 A B C D E F
  };
  unsigned char op = memory->getByte(address);
  return Instruction(mnemonic, lengths[op], memory->getBytes(address));
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x x x x x x x x x x x x x x x x 0
// 1 x x x x x x x x x x x x x x x x 1
// 2 x x x x x x x x x x x x x x x x 2
// 3 x x x x x x x x x x x x x x x x 3
// 4 . . . . . . . . . . . . . . . . 4
// 5 . . . . . . . . . . . . . . . . 5
// 6 . . . . . . . . . . . . . . . . 6
// 7 . . . . . . . . . . . . . . . . 7
// 8 x x x x x x x x x x x x x x x x 8
// 9 x x x x x x x x x x x x x x x x 9
// A x x x x x x x x x x x x x x x x A
// B x x x x x x x x x x x x x x x x B
// C . . . . . . . . . . . . . . . . C
// D . . . . . . . . . . . . . . . . D
// E . . . . . . . . . . . . . . . . E
// F . . . . . . . . . . . . . . . . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

Instruction
Disassembler::disassembleCB(unsigned short address) {
  unsigned char op = memory->getByte(address + 1);
  if(op >= 0x00 && op < 0x40) { // RLC, RRC, RL, RR, SLA, SRA, SWAP, SRL
    const std::string mnemonics[] = {"RLC", "RRC", "RL", "RR", "SLA", "SRA", "SWAP", "SRL"};
    return mkInstr(mnemonics[op >> 3] + " " + getRegName(op & 0x07), address);
  } else if((op & 0xff) >= 0x80 && (op & 0xff) < 0xc0) { // RES r
    int bit = (op - 0x80) >> 3;
    return mkInstr("RES " + std::to_string(bit) + "," + getRegName(op & 0x07), address);
  } else {
    return mkInstr("--- CB", address);
  }
}

std::string
Disassembler::getRegName(int reg) {
  const std::string regs[] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
  return regs[reg];
}

std::string
Disassembler::getWideRegNameAF(int widereg) {
  const std::string wideregs[] = {"BC", "DE", "HL", "AF"};
  return wideregs[widereg];
}

std::string
Disassembler::getWideRegNameSP(int widereg) {
  const std::string wideregs[] = {"BC", "DE", "HL", "SP"};
  return wideregs[widereg];
}

std::string
Disassembler::getIndirectRegName(int indirectreg) {
  const std::string indirectregs[] = {"(BC)", "(DE)", "(HL+)", "(HL-)"};
  return indirectregs[indirectreg];
}
