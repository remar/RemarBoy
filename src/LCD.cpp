#include "LCD.h"

const unsigned short IF = 0xFF0F;
const unsigned short LCDC = 0xFF40;
const unsigned short LY = 0xFF44;

LCD::LCD(Memory *memory) : mem(memory), cycles(0) {
  ly = mem->getByte(LY);
}

void
LCD::step() {
  cycles += mem->cycles;

  while(cycles > 114) {
    cycles -= 114;
    ly++;
    if(ly == 144) {
      // Vblank Interrupt Request
      mem->putByte(IF, mem->getByte(IF) | 0x01);
    }
    if(ly >= 154) {
      ly = 0;
    }
    if(ly == 0) {
      // Turn off Vblank Interrupt Request
      mem->putByte(IF, mem->getByte(IF) & ~0x01);
    }
    mem->putByte(LY, ly);
  }
}
