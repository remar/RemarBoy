#ifndef CONSOLE_H
#define CONSOLE_H

#include "DisassemblyEngine.h"

#include <string>
#include <vector>

class Console {
public:
  Console(DisassemblyEngine &engine);
  void run();

private:
  void execute(std::string command);

  void runSteps(std::vector<std::string> &tokens);
  void printSpeed();

  DisassemblyEngine &engine;
  bool quit;
};

#endif
