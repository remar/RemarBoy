#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "Emulator.h"
#include <string>

class Disassembler {
public:
  void insertCart(std::string path);
  void step();
private:
  Emulator emulator;
};

#endif
