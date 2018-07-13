#include "Timer.h"

const uint16_t DIV_REG = 0xFF04;

Timer::Timer(Memory *memory) : mem(memory), accumulatedCycles(0) {
  mem->putByte(DIV_REG, 0);
}

void
Timer::step() {
  accumulatedCycles += mem->cycles;

  if(accumulatedCycles >= 256) {
    accumulatedCycles -= 256;
    mem->putByte(DIV_REG, mem->getByte(DIV_REG) + 1);
  }
}
