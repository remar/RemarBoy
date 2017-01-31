#include "Memory.h"

// Memory map
// 0x0000-0x7FFF    Game rom (32 kB)
// 0x8000-0x9FFF    VRAM (8 kB)
// 0xA000-0xBFFF    External Expansion Working RAM (8 kB)
// 0xC000-0xDFFF    Unit Working RAM (8 kB)
// 0xE000-0xFDFF    (Use prohibited)
// 0xFE00-0xFE9F    OAM (160 bytes)
// 0xFF00-0xFF7F    Port/Mode/Control/Sound Registers
// 0xFF80-0xFFFE    Working & Stack RAM (127 bytes)
// 0xFFFF           Interrupt Enable

#include <iostream>
#include "Constants.h"

void
Memory::insertCart(std::string path) {
  cart = new Cart(path);
  cart->writeToMemory(mem, 0);
  cart->writeToMemory(&mem[BANK_SIZE], 1);
}

unsigned char
Memory::getByte(unsigned short address) {
  return mem[address];
}

void
Memory::putByte(unsigned short address, unsigned char byte) {
  if(address >= 0x8000) {
    mem[address] = byte;
  }
}


#if 0
int main() {
  Memory mem;
  mem.insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");
}
#endif
