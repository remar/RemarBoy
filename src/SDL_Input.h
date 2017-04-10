#ifndef SDL_INPUT_H
#define SDL_INPUT_H

#include "Memory.h"

enum KeyType {DOWN, UP, LEFT, RIGHT, START, SELECT, B, A};

class SDL_Input {
public:
  SDL_Input(Memory *memory);
  void step();
  bool quit();

private:
  void handleEvents();
  void interfaceToMemory();
  Memory *mem;
  bool shouldQuit;
  bool keys[8];
  int keysyms[8];
  bool reading;
};

#endif
