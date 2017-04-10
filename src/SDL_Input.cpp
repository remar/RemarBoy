#include "SDL_Input.h"

#include <SDL2/SDL.h>

const uint16_t P1 = 0xFF00;

SDL_Input::SDL_Input(Memory *memory) : mem(memory), shouldQuit(false), reading(false) {
  for(int i = 0;i < 8;i++) {
    keys[i] = false;
  }

  keysyms[0] = SDLK_DOWN;
  keysyms[1] = SDLK_UP;
  keysyms[2] = SDLK_LEFT;
  keysyms[3] = SDLK_RIGHT;
  keysyms[4] = SDLK_RETURN;
  keysyms[5] = SDLK_RSHIFT;
  keysyms[6] = SDLK_z;
  keysyms[7] = SDLK_x;
}

void
SDL_Input::step() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.type == SDL_QUIT) {
      shouldQuit = true;
    } else if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
      if(event.key.keysym.sym == SDLK_ESCAPE) {
	shouldQuit = true;
      }
      for(int i = 0;i < 8;i++) {
	if(event.key.keysym.sym == keysyms[i]) {
	  keys[i] = true;
	  break;
	}
      }
    } else if(event.type == SDL_KEYUP) {
      for(int i = 0;i < 8;i++) {
	if(event.key.keysym.sym == keysyms[i]) {
	  keys[i] = false;
	  break;
	}
      }
    }
  }

  uint8_t p1 = mem->getByte(P1) & 0x30;

  if(p1 == 0x10) {
    // Read START, SELECT, B, A
    p1 = (keys[START] ? 0 : 0x08) + (keys[SELECT] ? 0 : 0x04) + (keys[B] ? 0 : 0x02) + (keys[A] ? 0 : 0x01);
    mem->putByte(P1, p1);
    reading = true;
  } else if(p1 == 0x20) {
    // Read DOWN, UP, LEFT, RIGHT
    p1 = (keys[DOWN] ? 0 : 0x08) + (keys[UP] ? 0 : 0x04) + (keys[LEFT] ? 0 : 0x02) + (keys[RIGHT] ? 0 : 0x01);
    mem->putByte(P1, p1);
    reading = true;
  } else if(p1 == 0x30) {
    reading = false;
  }

  if(!reading) {
    mem->putByte(P1, 0xFF);
  }
}

bool
SDL_Input::quit() {
  return shouldQuit;
}
