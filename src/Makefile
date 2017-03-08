CXX := g++

CXXFLAGS := -std=c++11

OBJS = Cart.o Console.o Disassembler.o Analyzer.o Input.o Instruction.o LCD.o Memory.o RemarBoy.o

all: remarboy

CPU.cpp: generate_ops.py CPU.pre.cpp
	./generate_ops.py

CPU.o: CPU.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

remarboy: $(OBJS) CPU.o
	$(CXX) -o $@ $^

clean:
	rm -f *.o remarboy CPU.cpp
