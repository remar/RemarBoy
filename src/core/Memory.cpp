#include "Memory.h"
#include "Constants.h"
#include <iostream>
#include <cstring>

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

const uint8_t VISITED = 0x01;

Memory::Memory() : metarom(0) {}

void
Memory::insertCart(std::string path) {
  insertCart(new Cart(path));
}

void
Memory::insertCart(Cart *cart) {
  this->cart = cart;
  cart->writeToMemory(mem, 0);
  cart->writeToMemory(&mem[BANK_SIZE], 1);

  setupMetarom();

  memset(vram, 0, 8192);
}

uint8_t
Memory::getByte(uint16_t address) {
  return mem[address];
}

uint16_t
Memory::getWord(uint16_t address) {
  return mem[address] + (mem[address+1] << 8);
}

uint8_t
Memory::getOp(uint16_t address) {
  // TODO: Need more sophistication to deal with bank switching
  if(address < 0x8000) {
    metarom[address] = VISITED;
  }

  return mem[address];
}

void
Memory::putByte(uint16_t address, uint8_t byte) {
  if(address >= 0x8000) {
    mem[address] = byte;
  }
}

void
Memory::copyMemory(uint16_t dest, uint16_t src, uint16_t bytes) {
    memcpy(&mem[dest], &mem[src], bytes);
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

uint8_t*
Memory::getBytes(uint16_t address) {
  return &mem[address];
}

bool
Memory::vramChanged() {
  if(memcmp(vram, &mem[0x8000], 8192) != 0) {
    memcpy(vram, &mem[0x8000], 8192);
    return true;
  }
  return false;
}

void
Memory::setupMetarom() {
  if(metarom != 0) {
    delete[] metarom;
  }

  metarom = new uint8_t[cart->getSize()];
  for(int i = 0;i < cart->getSize();i++) {
    metarom[i] = 0;
  }
}
