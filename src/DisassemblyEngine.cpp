#include "DisassemblyEngine.h"

#include <chrono>
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
DisassemblyEngine::printDisassembly() {
  std::list<int> visited = memory.getVisited();
  std::list<int>::iterator it = visited.begin();
  for(;it != visited.end();it++) {
    std::cout << "[" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << *it << "] " << disassembler->disassemble(*it).asString() << std::endl;
  }
}

int
DisassemblyEngine::getCyclesPerSecond() {
  return cyclesPerSecondLastRun;
}
