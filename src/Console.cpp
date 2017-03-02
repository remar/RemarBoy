#include "Console.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

Console::Console(DisassemblyEngine &engine) : engine(engine), quit(false) {}

using namespace std;

void
Console::run() {
  cout << "RemarBoy -- Game Boy Interactive ROM Analyzer" << endl;

  string command;
  while(!quit) {
    cout << "> ";
    if(getline(cin, command)) {
      execute(command);
    } else {
      quit = true;
    }
  }
  cout << "Bye!" << endl;
}

void
Console::execute(string command) {
  istringstream iss(command);
  vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};

  if(tokens.empty()) {
    return;
  }

  string op = tokens[0];
  if(op == "quit" || op == "q" || op == "exit") {
    quit = true;
  } else if(op == "help") {
    cout << "{TODO: show list of commands here}" << endl;
  } else if(op == "run") {
    runSteps(tokens);
  } else if(op == "speed") {
    printSpeed();
  } else if(op == "print") {
    engine.printDisassembly();
  } else {
    cout << "Unrecognized command \"" << op << "\", try \"help\"" << endl;
  }
}

void
Console::runSteps(std::vector<std::string> &tokens) {
  if(tokens.size() == 1) {
    engine.runSteps(1);
  } else {
    int steps = 0;
    int multiplier = 1;

    if(toupper(tokens[1].back()) == 'M') {
      multiplier = 1000000;
    } else if(toupper(tokens[1].back()) == 'K') {
      multiplier = 1000;
    }

    try { 
      steps = stoi(tokens[1]);
    } catch(const out_of_range) {
      cout << "Number out of bounds (" << tokens[1] << ")" << endl;
      return;
    } catch(...) {
      cout << "Faulty argument to run (" << tokens[1] << ")" << endl;
      return;
    }
    engine.runSteps(steps * multiplier);
  }
}

void
Console::printSpeed() {
  int cycles = engine.getCyclesPerSecond();
  if(cycles == 0) {
    cout << "Cycles/sec: ?" << endl;
  } else {
    cout << "Cycles/sec: " << cycles << endl;
  }
}
