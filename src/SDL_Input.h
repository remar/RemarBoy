#ifndef SDL_INPUT_H
#define SDL_INPUT_H

#include "Memory.h"

class SDL_Input {
public:
  SDL_Input(Memory *memory);
  void step();
  bool quit();

private:
  Memory *mem;
  bool shouldQuit;
};

#endif
