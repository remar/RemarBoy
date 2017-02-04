#include "CPU.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

const unsigned char ZF = 0x80; // Zero flag
const unsigned char NF = 0x40; // Negative op flag
const unsigned char HF = 0x20; // Half-carry flag
const unsigned char CF = 0x10; // Carry flag

CPU::CPU(Memory* memory) : mem(memory) {
  PC = 0x100; // Program starts at 0x100
  SP = 0xfffe;
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x g . . . . g . . . . . . . g . 0
// 1 . g . . . . g . . . . . . . g . 1
// 2 . g . . . . g . . . . . . . g . 2
// 3 . . . . . . g . . . . . . . g . 3
// 4 . . . . . . . . . . . . . . . . 4
// 5 . . . . . . . . . . . . . . . . 5
// 6 . . . . . . . . . . . . . . . . 6
// 7 . . . . . . . . . . . . . . . . 7
// 8 . . . . . . . . . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A . . . . . . . . g g g g g g g g A
// B . . . . . . . . . . . . . . . . B
// C . . . x . . . . . . . . . . . . C
// D . . .   . . . . . . .   .   . . D
// E . . .     . . . . . .       . . E
// F . . . .   . . . . . . .     . . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

void
CPU::step() {
  unsigned char op = mem->getOp(PC++);
  switch(op) {
  case 0x00: // NOP
    mem->cycles = 1;
    break;

  case 0xC3: // JP nn
    PC = mem->getWord(PC);
    mem->cycles = 4;
    break;

// --------- BEGIN GENERATED CODE ---------
// --------- END GENERATED CODE ---------

  default:
    std::stringstream fmt;
    fmt << "OP not implemented: 0x"
	<< std::hex
	<< std::setw(2)
	<< std::setfill('0')
	<< std::uppercase
	<< (int)op;
    throw std::out_of_range(fmt.str());
  }
}
