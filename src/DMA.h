#ifndef DMA_H
#define DMA_H

#include "Memory.h"

class DMA {
public:
  DMA(Memory *memory);
  void step();

private:
  Memory *mem;
};

#endif
