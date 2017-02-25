#include "DisassemblyEngine.h"

#include <iostream>
#include <iomanip>

DisassemblyEngine::DisassemblyEngine() {
  emulator = new Emulator(&memory);
  disassembler = new Disassembler(&memory);
}

void
DisassemblyEngine::loadRom(std::string path) {
  memory.insertCart(path);
}

void
DisassemblyEngine::runSteps(int steps) {
  for(int i = 0;i < steps;i++) {
    emulator->step();
  }
}

void
DisassemblyEngine::printDisassembly() {
  std::list<int> visited = memory.getVisited();
  std::list<int>::iterator it = visited.begin();
  for(;it != visited.end();it++) {
    std::cout << "[" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << *it << "] " << disassembler->disassemble(*it).asString() << std::endl;
  }
}
