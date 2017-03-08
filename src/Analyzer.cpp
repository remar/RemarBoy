#include "Analyzer.h"

#include <chrono>
#include <iostream>
#include <iomanip>

Analyzer::Analyzer() {
  emulator = new Emulator(&memory);
  disassembler = new Disassembler(&memory);
}

void
Analyzer::loadRom(std::string path) {
  memory.insertCart(path);
}

void
Analyzer::runSteps(int steps) {
  auto start = std::chrono::high_resolution_clock::now();
  int cyclesStart = emulator->getTotalCPUCycles();
  for(int i = 0;i < steps;i++) {
    emulator->step();
  }
  auto end = std::chrono::high_resolution_clock::now();
  int cyclesEnd = emulator->getTotalCPUCycles();
  double ms = std::chrono::duration<double>(end - start).count();
  cyclesPerSecondLastRun = (cyclesEnd - cyclesStart) / ms;
}

void
Analyzer::printDisassembly() {
  std::list<int> visited = memory.getVisited();
  std::list<int>::iterator it = visited.begin();
  for(;it != visited.end();it++) {
    std::cout << "[" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << *it << "] " << disassembler->disassemble(*it).asString() << std::endl;
  }
}

int
Analyzer::getCyclesPerSecond() {
  return cyclesPerSecondLastRun;
}
