#include "SDL_LCD.h"

#include <cassert>
#include <iostream>
#include <cstring>

const uint16_t IF = 0xFF0F;
const uint16_t LCDC = 0xFF40;
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

  memset(screen, 0, screenWidth*screenHeight*4);
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
  getIntermediate();
  getBgChr();

  // Output background tile at (0,0) to top left corner of screen
  uint8_t lcdc = mem->getByte(LCDC);
  uint16_t offset = ((lcdc & 0x08) == 0x08) ? 0x9c00 : 0x9800;
  uint8_t chr = mem->getByte(offset);
  for(int y = 0;y < 8;y++) {
    memcpy(&screen[y*screenWidth*4], &bgChr[chr*256 + y*32], 32);
  }

  SDL_UpdateTexture(texture, 0, screen, screenWidth * 4);

  SDL_RenderCopy(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
}

void
SDL_LCD::getIntermediate() {
  for(int row = 0;row < 3072;row++) {
    uint8_t n0 = mem->getByte(0x8000 + row*2);
    uint8_t n1 = mem->getByte(0x8000 + row*2 + 1);

    for(int i = 0;i < 8;i++) {
      intermediate[row*8 + (7 - i)] = (n0 & 1) + ((n1 & 1) << 1);
      n0 >>= 1;
      n1 >>= 1;
    }
  }
}

void
SDL_LCD::getBgChr() {
  uint8_t lcdc = mem->getByte(LCDC);
  uint16_t offset = ((lcdc & 0x10) == 0x10) ? 0 : 0x800;

  for(int p = 0;p < 16384;p++) {
    uint8_t pixel = intermediate[p + offset];
    // TODO: Base color value off palette
    bgChr[p*4] = bgChr[p*4+1] = bgChr[p*4+2] = pixel * 85;
    bgChr[p*4+3] = SDL_ALPHA_OPAQUE;
  }
}
