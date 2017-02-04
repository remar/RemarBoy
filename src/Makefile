CXX := g++

OBJS = Cart.o CPU.o Emulator.o Memory.o

all: remarboy

remarboy: $(OBJS)
	$(CXX) -o $@ $^

clean:
	rm -f *.o remarboy
