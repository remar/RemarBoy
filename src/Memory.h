#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include "Cart.h"

class Memory {
public:
  int cycles;

  Memory();
  void insertCart(std::string path);
  unsigned char getByte(unsigned short address);
  unsigned long getWord(unsigned short address);
  unsigned char getOp(unsigned short address);
  void putByte(unsigned short address, unsigned char byte);

private:
  void setupMetarom();
  unsigned char mem[65536];
  unsigned char *metarom;
  Cart *cart;
};

#endif
