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
    try { 
      steps = stoi(tokens[1]);
    } catch(const out_of_range) {
      cout << "Number of out bounds (" << tokens[1] << ")" << endl;
      return;
    } catch(...) {
      cout << "Faulty argument to run (" << tokens[1] << ")" << endl;
      return;
    }
    engine.runSteps(steps);
  }
}
