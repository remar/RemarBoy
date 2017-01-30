#include "Cart.h"

#include <sys/stat.h>
#include <iostream>
#include <fstream>

Cart::Cart(std::string path) {
  size = getFileSize(path);

  data = new unsigned char[size];

  std::ifstream infile(path.c_str());

  infile.read((char *)data, size);
}

long
Cart::getFileSize(std::string path) {
  struct stat stat_buf;
  int rc = stat(path.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

#if 0
int main() {
  Cart cart("/home/andreas/Spel/roms/gb/Tetris.gb");
}
#endif
