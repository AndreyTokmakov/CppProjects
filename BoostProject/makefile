CC  = g++
CXX = g++

# Executable / Application file name:
EXECUTABLE = test

INC_DIR = /home/andtokm/Projects/eigen/Eigen

# compiler flags:
  #  -g     - this flag adds debugging information to the executable file
  #  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS = -c -Wall -std=c++2a -O3 -I$(INC_DIR)

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L.
LDFLAGS =

# libraries to link, i.e. -lm, -lz, -pthread and so on.
LDLIBS = -lm -pthread 

# TODO: ??????
DEPS = 

SOURCES  = main.cpp

# Object files ('.o' or '.obj' for *nix/win) wrapper:
OBJS     = $(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS)
