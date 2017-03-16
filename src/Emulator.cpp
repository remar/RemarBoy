#include "Memory.h"
#include "CPU.h"
#include "LCD.h"
#include "Input.h"

Memory *memory;
CPU *cpu;
LCD *lcd;
Input *input;

int main() {
  memory = new Memory();
  cpu = new CPU(memory);
  lcd = new LCD(memory);
  input = new Input(memory);

  memory->insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");

  // cpu->step();
  // lcd->step();
  // input->step();
}
