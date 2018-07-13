#ifndef TIMER_H
#define TIMER_H

#include "Memory.h"

class Timer {
public:
  Timer(Memory *memory);
  void step();

private:
  Memory *mem;
  int accumulatedCycles;
};

#endif
