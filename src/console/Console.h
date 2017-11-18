#ifndef CONSOLE_H
#define CONSOLE_H

#include "Analyzer.h"

#include <string>
#include <vector>

class Console {
public:
  Console(Analyzer &analyzer);
  void run();

private:
  void execute(std::string command);

  void runSteps(std::vector<std::string> &tokens);
  void printSpeed();

  Analyzer &analyzer;
  bool quit;
};

#endif
