#include "Disassembler.h"

#include <iostream>

Disassembler::Disassembler(Memory *memory) : memory(memory) {
  setupOpToMnemonicMap();
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x . . . . . . . . . . . . . . . 0
// 1 . . . . . . . . . . . . . . . . 1
// 2 . . . . . . . . . . . . . . . . 2
// 3 . . . . . . . . . . . . . . . . 3
// 4 . . . . . . . . . . . . . . . . 4
// 5 . . . . . . . . . . . . . . . . 5
// 6 . . . . . . . . . . . . . . . . 6
// 7 . . . . . . . . . . . . . . . . 7
// 8 . . . . . . . . . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A . . . . . . . . . . . . . . . . A
// B . . . . . . . . . . . . . . . . B
// C . . . . . . . . . . . . . . . . C
// D . . . . . . . . . . . . . . . . D
// E . . . . . . . . . . . . . . . . E
// F . . . . . . . . . . . . . . . . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

std::string
Disassembler::disassemble(unsigned short address) {
  unsigned char op = memory->getByte(address);

  std::string mnemonic = opToMnemonic[op];
  if(mnemonic != "") {
    return mnemonic;
  }

  return "---";
}

void
Disassembler::setupOpToMnemonicMap() {
  opToMnemonic[0x00] = "NOP";
}
