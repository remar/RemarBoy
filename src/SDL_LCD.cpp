#include "SDL_LCD.h"

#include <cassert>
#include <iostream>

const uint16_t IF = 0xFF0F;
const uint16_t SDL_LCDC = 0xFF40;
const uint16_t LY = 0xFF44;

SDL_LCD::SDL_LCD(Memory *memory) : mem(memory), cycles(0) {
  ly = mem->getByte(LY);

  window = SDL_CreateWindow("GAME BOY",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    screenWidth, screenHeight,
			    SDL_WINDOW_SHOWN);
  assert(window != 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  assert(renderer != 0);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			      SDL_TEXTUREACCESS_STREAMING,
			      screenWidth, screenHeight);
  assert(texture != 0);

  pixels = new uint8_t[screenWidth*screenHeight*4];
}

void
SDL_LCD::step() {
  cycles += mem->cycles;

  while(cycles > 114) {
    cycles -= 114;
    ly++;

    if(ly == 144) {
      // Vblank Interrupt Request
      mem->putByte(IF, mem->getByte(IF) | 0x01);
      redraw();
    }
    if(ly >= 154) {
      ly = 0;
    }
    if(ly == 0) {
      // Turn off Vblank Interrupt Request
      mem->putByte(IF, mem->getByte(IF) & ~0x01);
    }
    mem->putByte(LY, ly);
  }
}

void
SDL_LCD::redraw() {
  unsigned int x = screenWidth/2;
  unsigned int y = screenHeight/2;
  unsigned int offset = (screenWidth*4*y) + x*4;
  pixels[offset] = 0;
  pixels[offset+1] = 0;
  pixels[offset+2] = 255;
  pixels[offset+3] = SDL_ALPHA_OPAQUE;

  SDL_UpdateTexture(texture, 0, pixels, screenWidth * 4);

  SDL_RenderCopy(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
}
