#include "Memory.h"
#include "CPU.h"
#include "DMA.h"
#include "SDL_LCD.h"
#include "SDL_Input.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include <iostream>
#include <fstream>

Memory *memory;
CPU *cpu;
DMA *dma;
SDL_LCD *lcd;
SDL_Input *input;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    std::cout << "Usage: emulator <rom path>" << std::endl;
    return 0;
  }

  std::ifstream file(argv[1]);
  if(!file.good()) {
    std::cout << "Error: File doesn't exist (" << argv[1] << ")" << std::endl;
    return -1;
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  atexit(SDL_Quit);

  memory = new Memory();
  cpu = new CPU(memory);
  dma = new DMA(memory);
  lcd = new SDL_LCD(memory);
  input = new SDL_Input(memory);

  std::cout << "Loading ROM: " << argv[1] << std::endl;
  memory->insertCart(argv[1]);

  while(!input->quit()) {
    cpu->step();
    dma->step();
    lcd->step();
    input->step();
  }
}
