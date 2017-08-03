#ifndef MOCK_CART_H
#define MOCK_CART_H

#include "Cart.h"

class MockCart : public Cart {
public:
  MockCart(int size);
  void putByte(uint16_t address, uint8_t byte);
};

#endif
