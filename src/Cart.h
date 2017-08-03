#ifndef CART_H
#define CART_H

#include <string>

class Cart {
public:
  Cart();
  Cart(std::string path);
  void writeToMemory(uint8_t* memory, int bank);
  int getSize();

protected:
  uint8_t *data;
  int size;
  long getFileSize(std::string path);
};

#endif
