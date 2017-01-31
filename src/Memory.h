#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include "Cart.h"

class Memory {
public:
  void insertCart(std::string path);
  unsigned char getByte(unsigned short address);
  void putByte(unsigned short address, unsigned char byte);

private:
  unsigned char mem[65536];
  Cart *cart;
};

#endif
