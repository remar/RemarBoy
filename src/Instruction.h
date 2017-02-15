#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

class Instruction {
public:
  Instruction(std::string mnemonic, int len, unsigned char *data);
  std::string asString();

private:
  std::string mnemonic;
  int len;
  unsigned char data[3];
};

#endif
