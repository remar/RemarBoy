#include "SDL_LCD.h"

#include <cassert>
#include <iostream>
#include <cstring>

const uint16_t IF = 0xFF0F;
const uint16_t LCDC = 0xFF40;
const uint16_t LY = 0xFF44;
const uint16_t BGP = 0xFF47;
const uint16_t OBP0 = 0xFF48;
const uint16_t OBP1 = 0xFF49;

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
  if(mem->vramChanged()) {
    getIntermediate();
    getObj0Chr();
    getObj1Chr();
    getBgChr();
    renderBg(bg0, 0x9800);
    renderBg(bg1, 0x9c00);
  }

  uint8_t lcdc = mem->getByte(LCDC);

  bool drawBg = (lcdc & 0x01) == 0x01;
  if(drawBg) {
    uint8_t *bg = ((lcdc & 0x08) == 0x08) ? bg1 : bg0;

    for(int y = 0;y < screenHeight;y++) {
      memcpy(&screen[y*screenWidth*4], &bg[y*256*4], screenWidth*4);
    }
  }

  if((lcdc & 0x02) == 0x02) {
    OBJ *obj = (OBJ*)mem->getBytes(0xFE00);
    for(int i = 0;i < 40;i++) {
      uint8_t *objChr = ((obj[i].attr & 0x10) == 0x10) ? obj1Chr : obj0Chr;
      uint8_t obj_y = obj[i].y - 16;
      uint8_t obj_x = obj[i].x - 8;
      for(int y = 0;y < 8;y++) {
	memcpy(&screen[((y + obj_y) * screenWidth + obj_x) * 4],
	       &objChr[obj[i].chr * 256 + y * 32],
	       32);
      }
    }
  }

  SDL_UpdateTexture(texture, 0, screen, screenWidth * 4);

  SDL_RenderCopy(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
}

void
SDL_LCD::getIntermediate() {
  uint8_t *bytes = mem->getBytes(0x8000);
  for(int row = 0;row < 3072;row++) {
    uint8_t n0 = *bytes; bytes++;
    uint8_t n1 = *bytes; bytes++;

    for(int i = 0;i < 8;i++) {
      intermediate[row*8 + (7 - i)] = (n0 & 1) + ((n1 & 1) << 1);
      n0 >>= 1;
      n1 >>= 1;
    }
  }
}

void
SDL_LCD::getObj0Chr() {
  getObj0Palette();

  for(int p = 0;p < 16384;p++) {
    uint8_t pixel = intermediate[p];
    obj0Chr[p*4] = obj0Chr[p*4+1] = obj0Chr[p*4+2] = obj0Pal[pixel] * 85;
    obj0Chr[p*4+3] = SDL_ALPHA_OPAQUE;
  }
}

void
SDL_LCD::getObj0Palette() {
  uint8_t obp0 = mem->getByte(OBP0);
  for(int i = 0;i < 4;i++) {
    obj0Pal[i] = 3 - (obp0 & 3);
    obp0 >>= 2;
  }
}

void
SDL_LCD::getObj1Chr() {
  getObj1Palette();

  for(int p = 0;p < 16384;p++) {
    uint8_t pixel = intermediate[p];
    obj1Chr[p*4] = obj1Chr[p*4+1] = obj1Chr[p*4+2] = obj1Pal[pixel] * 85;
    obj1Chr[p*4+3] = SDL_ALPHA_OPAQUE;
  }
}

void
SDL_LCD::getObj1Palette() {
  uint8_t obp1 = mem->getByte(OBP1);
  for(int i = 0;i < 4;i++) {
    obj1Pal[i] = 3 - (obp1 & 3);
    obp1 >>= 2;
  }
}

void
SDL_LCD::getBgChr() {
  uint8_t lcdc = mem->getByte(LCDC);
  uint16_t offset = ((lcdc & 0x10) == 0x10) ? 0 : 0x800;

  getBgPalette();

  for(int p = 0;p < 16384;p++) {
    uint8_t pixel = intermediate[p + offset];
    bgChr[p*4] = bgChr[p*4+1] = bgChr[p*4+2] = bgPal[pixel] * 85;
    bgChr[p*4+3] = SDL_ALPHA_OPAQUE;
  }
}

void
SDL_LCD::getBgPalette() {
  uint8_t bgp = mem->getByte(BGP);
  for(int i = 0;i < 4;i++) {
    bgPal[i] = 3 - (bgp & 3);
    bgp >>= 2;
  }
}

void
SDL_LCD::renderBg(uint8_t *bg, uint16_t offset) {
  for(int bg_y = 0;bg_y < 32;bg_y++) {
    for(int bg_x = 0;bg_x < 32;bg_x++) {
      uint8_t chr = mem->getByte(offset + bg_y*32 + bg_x);
      for(int y = 0;y < 8;y++) {
	memcpy(&bg[((bg_y*8 + y)*256 + bg_x*8)*4], &bgChr[chr*256 + y*32], 32);
      }
    }
  }
}
