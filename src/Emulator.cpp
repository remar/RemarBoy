#include "Memory.h"
#include "CPU.h"
#include "SDL_LCD.h"
#include "SDL_Input.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

Memory *memory;
CPU *cpu;
SDL_LCD *lcd;
SDL_Input *input;

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  atexit(SDL_Quit);

  memory = new Memory();
  cpu = new CPU(memory);
  lcd = new SDL_LCD(memory);
  input = new SDL_Input(memory);

  memory->insertCart("/home/andreas/Spel/roms/gb/Tetris.gb");

  while(!input->quit()) {
    cpu->step();
    lcd->step();
    input->step();
  }
}
