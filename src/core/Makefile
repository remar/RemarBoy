AR := ar
CPPFILES := $(wildcard *.cpp) CPU.cpp
DEPFILES := $(patsubst %.cpp,%d,$(CPPFILES))
CXXFLAGS := -Wall -std=c++11
OFILES := $(CPPFILES:.cpp=.o)

libcore.a: $(OFILES)
	$(AR) rvs $@ $^

-include $(DEPFILES)

CPU.cpp: CPU.pre generate_ops.py
	./generate_ops.py

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	@rm -f CPU.cpp *.o *.d libcore.a

