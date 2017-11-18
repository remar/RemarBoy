CORE := ../core
LIBCORE := $(CORE)/libcore.a
LDLIBS := -lcore $(EXTRALIBS)
LDFLAGS := -L$(CORE)

CPPFILES := $(wildcard *.cpp)
DEPFILES := $(patsubst %.cpp,%.d,$(CPPFILES))
OFILES := $(CPPFILES:.cpp=.o)

CXXFLAGS := -Wall -std=c++11 -I$(CORE)

$(EXE): $(OFILES)
	$(MAKE) -C $(CORE)
	$(CXX) -o $@ $(OFILES) $(LDFLAGS) $(LDLIBS)

-include $(DEPFILES)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	@rm -f *.o $(EXE) *.d

