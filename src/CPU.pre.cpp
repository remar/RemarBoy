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
  accum_cycles = 0;
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 x g g g g g g . . g g g g g g . 0
// 1 . g g g g g g . x g g g g g g . 1
// 2 g g g g g g g . g g g g g g g x 2
// 3 g x g . x x g . g g g . g g g . 3
// 4 g g g g g g g g g g g g g g g g 4
// 5 g g g g g g g g g g g g g g g g 5
// 6 g g g g g g g g g g g g g g g g 6
// 7 g g g g g g . g g g g g g g g g 7
// 8 g g g g g g g g . . . . . . . . 8
// 9 . . . . . . . . . . . . . . . . 9
// A g g g g g g g g g g g g g g g g A
// B g g g g g g g g . . . . . . . . B
// C g g g x . g x g g x g . . x . g C
// D g g g   . g . g g x g   .   . g D
// E x g x     g x g . x x       . g E
// F x g . x   g . g . . x x     x g F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

void
CPU::step() {
  // std::cout << std::hex << PC << std::endl;

  if(IME) {
    unsigned char IF = mem->getByte(0xFF0F); // Interrupt Request
    unsigned char IE = mem->getByte(0xFFFF); // Interrupt Enable
    unsigned char i = 0x01;
    unsigned char j = 0;
    while(i < 0x20) {
      if((IF & i) != 0 && (IE & i) != 0) {
	IME = false;
	mem->putByte(0xFF0F, IF & ~i); // Reset Interrupt Request bit
	mem->putByte(SP-1, (PC & 0xff00) >> 8);
	mem->putByte(SP-2, PC & 0x00ff);
	SP -= 2;
	PC = 0x0040 + j * 0x08; // Jump to interrupt routine
	return;
      }
      i <<= 1;
      j++;
    }
  }

  unsigned char op = mem->getOp(PC++);
  unsigned char n;
  bool carry;
  bool halfcarry;

  switch(op) {
  case 0x00: // NOP
    mem->cycles = 1;
    break;

  case 0x18: // JR n
    PC += (signed char)mem->getByte(PC) + 1;
    mem->cycles = 3;
    break;

  case 0x2F: // CPL
    AF.high ^= 0xff;
    AF.low = (AF.low & (ZF | CF)) | NF | HF;
    mem->cycles = 1;
    break;

  case 0x31: // LD SP,nn
    SP = mem->getWord(PC);
    PC += 2;
    mem->cycles = 3;
    break;

  case 0x34: // INC (HL)
    n = mem->getByte(HL.word) + 1;
    AF.low = (AF.low & CF) | (n == 0 ? ZF : 0) | (n == 0x10 ? HF : 0);
    mem->putByte(HL.word, n);
    mem->cycles = 3;
    break;

  case 0x35: // DEC (HL)
    n = mem->getByte(HL.word) - 1;
    AF.low = (AF.low & CF) | (n == 0 ? ZF : 0) | NF | ((n & 0x0f) == 0x0f ? HF : 0);
    mem->putByte(HL.word, n);
    mem->cycles = 3;
    break;

  case 0xC3: // JP nn
    PC = mem->getWord(PC);
    mem->cycles = 4;
    break;

  case 0xC6: // ADD n
    n = mem->getByte(PC++);
    halfcarry = (AF.high & 0x0f) + (n & 0x0f) > 0x0f;
    carry = (AF.high + n) > 0xff;
    AF.high += n;
    AF.low = (AF.high == 0 ? ZF : 0) | (carry ? CF : 0) | (halfcarry ? HF : 0);
    mem->cycles = 2;
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

  case 0xD9: // RETI
    IME = true;
    PC = mem->getWord(SP);
    SP += 2;
    mem->cycles = 4;
    break;

  case 0xE0: // LD (0xff00 + n),A
    mem->putByte(0xff00 + mem->getByte(PC++), AF.high);
    mem->cycles = 3;
    break;

  case 0xE2: // LD (0xff00 + C),A
    mem->putByte(0xff00 + BC.low, AF.high);
    mem->cycles = 2;
    break;

  case 0xE6: // AND n
    AF.high &= mem->getByte(PC++);
    AF.low = (AF.high == 0 ? ZF : 0) | HF;
    mem->cycles = 2;
    break;

  case 0xE9: // JP (HL)
    PC = HL.word;
    mem->cycles = 1;
    break;

  case 0xEA: // LD (nn),A
    mem->putByte(mem->getWord(PC), AF.high);
    PC += 2;
    mem->cycles = 4;
    break;

  case 0xF0: // LD A,(0xff00 + n)
    AF.high = mem->getByte(0xff00 + mem->getByte(PC++));
    mem->cycles = 3;
    break;

  case 0xF3: // DI
    IME = false;
    mem->cycles = 1;
    break;

  case 0xFA: // LD A,(nn)
    AF.high = mem->getByte(mem->getWord(PC));
    PC += 2;
    mem->cycles = 4;
    break;

  case 0xFB: // EI
    IME = true;
    mem->cycles = 1;
    break;

  case 0xFE: // CP n
    n = mem->getByte(PC++);
    AF.low = (AF.high == n ? ZF : 0) | NF | ((AF.high & 0xf) < (n & 0xf) ? HF : 0) | (AF.high < n ? CF : 0);
    mem->cycles = 2;
    break;

// --------- BEGIN GENERATED CODE ---------
// --------- END GENERATED CODE ---------

  default:
    printState();
    std::stringstream fmt;
    fmt << "OP not implemented: 0x"
	<< std::hex
	<< std::setw(2)
	<< std::setfill('0')
	<< std::uppercase
	<< (int)op;
    throw std::out_of_range(fmt.str());
  }

  accum_cycles += mem->cycles;
}

//   0 1 2 3 4 5 6 7 8 9 A B C D E F
// 0 . . . . . . . . . . . . . . . . 0
// 1 . . . . . . . . . . . . . . . . 1
// 2 g g g g g g g g . . . . . . . . 2
// 3 g g g g g g g g . . . . . . . . 3
// 4 g g g g g g g g g g g g g g g g 4
// 5 g g g g g g g g g g g g g g g g 5
// 6 g g g g g g g g g g g g g g g g 6
// 7 g g g g g g g g g g g g g g g g 7
// 8 g g g g g g g g g g g g g g g g 8
// 9 g g g g g g g g g g g g g g g g 9
// A g g g g g g g g g g g g g g g g A
// B g g g g g g g g g g g g g g g g B
// C . . . . . . . . . . . . . . . . C
// D . . . . . . . . . . . . . . . . D
// E . . . . . . . . . . . . . . . . E
// F . . . . . . . . . . . . . . . . F
//   0 1 2 3 4 5 6 7 8 9 A B C D E F

void
CPU::doCB() {
  unsigned char op = mem->getByte(PC++);
  unsigned char temp;
  bool carry;

  switch(op) {
// --------- BEGIN GENERATED CB CODE ---------
// --------- END GENERATED CB CODE ---------
  default:
    printState();
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
  std::cout << "------------------" << std::endl;
  printWordReg("PC", PC);
  std::cout << "  ";
  printWordReg("SP", SP);
  std::cout << std::endl;

  printWordReg("AF", AF.word);
  std::cout << "  ";
  printWordReg("BC", BC.word);
  std::cout << std::endl;
  printWordReg("DE", DE.word);
  std::cout << "  ";
  printWordReg("HL", HL.word);
  std::cout << std::endl;

  std::cout << "Total Cycles: " << std::dec << accum_cycles << std::endl;
}

void
CPU::printWordReg(std::string name, unsigned short value) {
  std::cout << name << ": " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << value;
}
