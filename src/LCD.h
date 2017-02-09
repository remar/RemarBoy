#ifndef LCD_H
#define LCD_H

#include "Memory.h"

class LCD {
public:
  LCD(Memory *memory);
  void step();

private:
  Memory *mem;
  int cycles;
  int ly;
};

#endif
