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
  uint8_t getByte(uint16_t address);
  uint16_t getWord(uint16_t address);
  uint8_t getOp(uint16_t address);
  void putByte(uint16_t address, uint8_t byte);
  std::list<int> getVisited();
  uint8_t* getBytes(uint16_t address);

private:
  void setupMetarom();
  uint8_t mem[65536];
  uint8_t *metarom;
  Cart *cart;
};

#endif
