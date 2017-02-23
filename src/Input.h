#ifndef INPUT_H
#define INPUT_H

#include "Memory.h"

class Input {
public:
  Input(Memory *memory);
  void step();

private:
  Memory *mem;
};

#endif
