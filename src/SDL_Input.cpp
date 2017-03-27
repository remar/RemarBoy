#include "SDL_Input.h"

#include <SDL2/SDL.h>

SDL_Input::SDL_Input(Memory *memory) : mem(memory), shouldQuit(false) {}

void
SDL_Input::step() {
  mem->putByte(0xFF00, 0xFF);

  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.type == SDL_QUIT) {
      shouldQuit = true;
    }
  }
}

bool
SDL_Input::quit() {
  return shouldQuit;
}

