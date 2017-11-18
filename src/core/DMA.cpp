#include "DMA.h"

#include <iostream>

const uint16_t DMA_REG = 0xFF46;
const uint16_t OAM = 0xFE00;

DMA::DMA(Memory *memory) : mem(memory) {
  mem->putByte(DMA_REG, 0);
}

void
DMA::step() {
  uint16_t source = mem->getByte(DMA_REG) * 0x100;

  if(source != 0) {
    mem->copyMemory(OAM, source, 160);
    mem->putByte(DMA_REG, 0);
  }
}
