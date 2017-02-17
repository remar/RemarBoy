#include "Instruction.h"
#include <sstream>
#include <iomanip>

Instruction::Instruction(std::string mnemonic, int len, unsigned char *data)
  : mnemonic(mnemonic), len(len) {
  for(int i = 0;i < len;i++) {
    this->data[i] = data[i];
  }
}

std::string
Instruction::asString() {
  std::stringstream fmt;
  fmt << std::setfill(' ') << std::setw(15) << std::left << mnemonic << std::right;
  if(len > 0) {
    fmt << "   (";
    for(int i = 0;i < len - 1;i++) {
      fmt << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << (int)data[i] << ", ";
    }
    fmt << std::hex << std::setfill('0') << std::uppercase << std::setw(2) << (int)data[len - 1] << ")";
  }
  return fmt.str();
}
