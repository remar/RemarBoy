#ifndef SDL_LCD_H
#define SDL_LCD_H

#include "Memory.h"

#include <SDL2/SDL.h>
#include <vector>

const uint16_t screenWidth = 160;
const uint16_t screenHeight = 144;

class SDL_LCD {
public:
  SDL_LCD(Memory *memory);
  void step();

private:
  void redraw();
  void getIntermediate();
  void getBgChr();
  void getBgPalette();
  void renderBg(uint8_t *bg, uint16_t offset);

  Memory *mem;
  int cycles;
  int ly;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint8_t screen[screenWidth*screenHeight*4];
  uint8_t intermediate[24 * 1024];
  uint8_t obj0Chr[64 * 1024];
  uint8_t obj1Chr[64 * 1024];
  uint8_t bgChr[64 * 1024];
  uint8_t bgPal[4];
  uint8_t bg0[256 * 1024];
  uint8_t bg1[256 * 1024];
};

#endif
