#ifndef CART_H
#define CART_H

#include <string>

class Cart {
public:
  Cart(std::string path);

private:
  unsigned char *data;
  long size;
  long getFileSize(std::string path);
};

#endif
