#include "Analyzer.h"
#include "Console.h"

Analyzer analyzer;

int main() {
  Console console(analyzer);

  analyzer.loadRom("/home/andreas/Spel/roms/gb/Tetris.gb");

  console.run();
}
