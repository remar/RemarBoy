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
  IME = true; // TODO: Verify that it's on at start
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x g g . g g g . . . g g g g g . 0
// 1 . g g . g g g . . . g g g g g . 1
// 2 g g g . g g g . g . g g g g g x 2
// 3 g x g . . . g . g . g . g g g . 3
// 4 g g g g g g g g g g g g g g g g 4
// 5 g g g g g g g g g g g g g g g g 5
// 6 g g g g g g g g g g g g g g g g 6
// 7 . . . . . . . . g g g g g g g g 7
// 8 g g g g g g g g . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A g g g g g g g g g g g g g g g g A
// B g g g g g g g g . . . . . . . . B
// C . . . x . . . g . x . . . x . g C
// D . . .   . . . g . . .   .   . g D
// E x . x     . x g . . x       . g E
// F x . . x   . . g . . . x     x g F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

void
CPU::step() {
  // std::cout << std::hex << PC << std::endl;
  unsigned char op = mem->getOp(PC++);
  unsigned char n;
  bool carry;
  bool halfcarry;

  switch(op) {
  case 0x00: // NOP
    mem->cycles = 1;
    break;

  case 0x2F: // CPL
    A ^= 0xff;
    F = (F & (ZF | CF)) | NF | HF;
    mem->cycles = 1;
    break;

  case 0x31: // LD SP,nn
    SP = mem->getWord(PC);
    PC += 2;
    mem->cycles = 3;
    break;

  case 0xC3: // JP nn
    PC = mem->getWord(PC);
    mem->cycles = 4;
    break;

  case 0xC9: // RET
    PC = mem->getWord(SP);
    SP += 2;
    mem->cycles = 4;
    break;

  case 0xCB: // CB prefix
    doCB();
    break;

  case 0xCD: // CALL nn
    PC += 2;
    mem->putByte(SP-1, (PC & 0xff00) >> 8);
    mem->putByte(SP-2, PC & 0x00ff);
    SP -= 2;
    PC = mem->getWord(PC - 2);
    mem->cycles = 6;
    break;

  case 0xE0: // LD (0xff00 + n),A
    mem->putByte(0xff00 + mem->getByte(PC++), A);
    mem->cycles = 3;
    break;

  case 0xE2: // LD (0xff00 + C),A
    mem->putByte(0xff00 + C, A);
    mem->cycles = 2;
    break;

  case 0xE6: // AND n
    A &= mem->getByte(PC++);
    F = (A == 0 ? ZF : 0) | HF;
    mem->cycles = 2;
    break;

  case 0xEA: // LD (nn),A
    mem->putByte(mem->getWord(PC), A);
    PC += 2;
    mem->cycles = 4;
    break;

  case 0xF0: // LD A,(0xff00 + n)
    A = mem->getByte(0xff00 + mem->getByte(PC++));
    mem->cycles = 3;
    break;

  case 0xF3: // DI
    IME = false;
    mem->cycles = 1;
    break;

  case 0xFB: // EI
    IME = true;
    mem->cycles = 1;
    break;

  case 0xFE: // CP n
    n = mem->getByte(PC++);
    F = (A == n ? ZF : 0) | NF | ((A & 0xf) < (n & 0xf) ? HF : 0) | (A < n ? CF : 0);
    mem->cycles = 2;
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

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 . . . . . . . . . . . . . . . . 0
// 1 . . . . . . . . . . . . . . . . 1
// 2 . . . . . . . . . . . . . . . . 2
// 3 g g g g g g g g . . . . . . . . 3
// 4 . . . . . . . . . . . . . . . . 4
// 5 . . . . . . . . . . . . . . . . 5
// 6 . . . . . . . . . . . . . . . . 6
// 7 . . . . . . . . . . . . . . . . 7
// 8 . . . . . . . . . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A . . . . . . . . . . . . . . . . A
// B . . . . . . . . . . . . . . . . B
// C . . . . . . . . . . . . . . . . C
// D . . . . . . . . . . . . . . . . D
// E . . . . . . . . . . . . . . . . E
// F . . . . . . . . . . . . . . . . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

void
CPU::doCB() {
  unsigned char op = mem->getByte(PC++);
  unsigned char temp;

  switch(op) {
// --------- BEGIN GENERATED CB CODE ---------
// --------- END GENERATED CB CODE ---------
  default:
    std::stringstream fmt;
    fmt << "CB OP not implemented: 0x"
	<< std::hex
	<< std::setw(2)
	<< std::setfill('0')
	<< std::uppercase
	<< (int)op;
    throw std::out_of_range(fmt.str());
  }
}

void
CPU::printState() {
  printWordReg("PC", PC);
  std::cout << "  ";
  printWordReg("SP", SP);
  std::cout << std::endl;

  printRegPair("A", A, "F", F);
  printRegPair("B", B, "C", C);
  printRegPair("D", D, "E", E);
  printRegPair("H", H, "L", L);
}

void
CPU::printByteReg(std::string name, unsigned char value) {
  std::cout << name << ": " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)value;
}

void
CPU::printWordReg(std::string name, unsigned short value) {
  std::cout << name << ": " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << value;
}

void
CPU::printRegPair(std::string name1, unsigned char value1,
		  std::string name2, unsigned char value2) {
  printByteReg(name1, value1);
  std::cout << "     ";
  printByteReg(name2, value2);
  std::cout << std::endl;
}
