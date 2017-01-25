#ifndef MEMORY_H
#define MEMORY_H

class Memory {
 private:
  unsigned char mem[65536];

 public:
  unsigned char getByte(unsigned short address);
  void putByte(unsigned short address, unsigned char byte);
};

#endif
