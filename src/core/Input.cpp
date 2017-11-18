#include "Input.h"

Input::Input(Memory *memory) : mem(memory) {}

void
Input::step() {
  mem->putByte(0xFF00, 0xFF);
}
