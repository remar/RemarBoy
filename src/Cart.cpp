#include "Cart.h"

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include "Constants.h"

Cart::Cart(std::string path) {
  size = getFileSize(path);

  data = new unsigned char[size];

  std::ifstream infile(path.c_str());

  infile.read((char *)data, size);
}

void
Cart::writeToMemory(unsigned char *memory, int bank) {
  memcpy(memory, &data[bank * BANK_SIZE], BANK_SIZE);
}

long
Cart::getSize() {
  return size;
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
