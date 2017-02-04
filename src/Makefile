CXX := g++

OBJS = Cart.o Disassembler.o Emulator.o Memory.o

all: remarboy

CPU.cpp: generate_ops.py CPU.pre.cpp
	./generate_ops.py

CPU.o: CPU.cpp
	$(CXX) -c -o $@ $^

remarboy: $(OBJS) CPU.o
	$(CXX) -o $@ $^

clean:
	rm -f *.o remarboy CPU.cpp
