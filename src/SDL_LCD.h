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

  Memory *mem;
  int cycles;
  int ly;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint8_t *pixels;
};

#endif
