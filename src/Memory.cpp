#include "Memory.h"
#include "Constants.h"
#include <iostream>

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

const unsigned char VISITED = 0x01;

Memory::Memory() : metarom(0) {}

void
Memory::insertCart(std::string path) {
  cart = new Cart(path);
  cart->writeToMemory(mem, 0);
  cart->writeToMemory(&mem[BANK_SIZE], 1);

  setupMetarom();
}

unsigned char
Memory::getByte(unsigned short address) {
  return mem[address];
}

unsigned short
Memory::getWord(unsigned short address) {
  return mem[address] + (mem[address+1] << 8);
}

unsigned char
Memory::getOp(unsigned short address) {
  // TODO: Need more sophistication to deal with bank switching
  metarom[address] = VISITED;

  return mem[address];
}

void
Memory::putByte(unsigned short address, unsigned char byte) {
  if(address >= 0x8000) {
    mem[address] = byte;
  }
}

std::list<int>
Memory::getVisited() {
  std::list<int> visited;

  for(int i = 0;i < cart->getSize();i++) {
    if(metarom[i] == VISITED) {
      visited.push_back(i);
    }
  }

  return visited;
}

void
Memory::setupMetarom() {
  if(metarom != 0) {
    delete[] metarom;
  }

  metarom = new unsigned char[cart->getSize()];
  for(int i = 0;i < cart->getSize();i++) {
    metarom[i] = 0;
  }
}
