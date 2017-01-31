#ifndef CART_H
#define CART_H

#include <string>

class Cart {
public:
  Cart(std::string path);
  void writeToMemory(unsigned char* memory, int bank);

private:
  unsigned char *data;
  long size;
  long getFileSize(std::string path);
};

#endif
