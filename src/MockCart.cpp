#include "MockCart.h"

MockCart::MockCart(int size) {
  data = new uint8_t[size];
}

void
MockCart::putByte(uint16_t address, uint8_t byte) {
  data[address] = byte;
}
