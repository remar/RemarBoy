#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <list>
#include "Cart.h"

class Memory {
public:
  int cycles;

  Memory();
  void insertCart(std::string path);
  void insertCart(Cart *cart);
  uint8_t getByte(uint16_t address);
  uint16_t getWord(uint16_t address);
  uint8_t getOp(uint16_t address);
  void putByte(uint16_t address, uint8_t byte);
  void copyMemory(uint16_t dest, uint16_t src, uint16_t bytes);
  std::list<int> getVisited();
  uint8_t* getBytes(uint16_t address);
  bool vramChanged();

private:
  void setupMetarom();
  uint8_t mem[65536];
  uint8_t *metarom;
  Cart *cart;
  uint8_t vram[8192];
};

#endif
