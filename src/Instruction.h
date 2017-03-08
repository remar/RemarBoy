#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

class Instruction {
public:
  Instruction(std::string mnemonic, int len, uint8_t *data);
  std::string asString();

private:
  std::string mnemonic;
  int len;
  uint8_t data[3];
};

#endif
