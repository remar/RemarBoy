#ifndef CART_H
#define CART_H

#include <string>

class Cart {
public:
  Cart(std::string path);
  void writeToMemory(unsigned char* memory, int bank);
  int getSize();

private:
  unsigned char *data;
  int size;
  long getFileSize(std::string path);
};

#endif
