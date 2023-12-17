# The PROGRAM macro defines the name of the program or project.
# It allows the program name to be changed by editing in only one location

PROGRAM = unclosed_descriptors_bin

# Command line parameters: Edit these parameters so that you can 
# easily run the sample by typing "make -f Makefile.Linux run".
# 
# You will need to:
# (1) Describe parameters here. ex: IN_SHAPEFILE is the input shapefile
# (2) Define parameters below this comment box.  ex: IN_SHAPEFILE = /mycomp/data/shapefile.shp
# (3) Add the parameters to the run target at the end of this file  ex: ./$(PROGRAM) $(IN_SHAPEFILE)
#
# The INCLUDEDIRS macro contains a list of include directories to pass to the compiler so it can find necessary header files.
# The LIBDIRS macro contains a list of library directories to pass to the linker so it can find necessary libraries.
# The LIBS macro contains a list of libraries that the the executable must be linked against.

INCLUDEDIRS = 
LIBDIRS = 
LIBS = # -lpthread

# The CXXSOURCES macro contains a list of source files.
# The CXXOBJECTS macro converts the CXXSOURCES macro into a list of object files.
# The CXXFLAGS macro contains a list of options to be passed to the compiler.
# 			   Adding "-g" to this line will cause the compiler o add debugging information to the executable.
#
# The CXX macro defines the C++ compiler.
# The LDFLAGS macro contains all of the library and library directory information to be passed to the linker.

CXXSOURCES = main.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)  # expands to list of object files
CXXFLAGS = -std=c++23 -DESRI_UNIX $(INCLUDEDIRS)
CXXFLAGS += -DDEBUG -ggdb -gdwarf-3
CXX = g++-13.1

LDFLAGS = $(LIBDIRS) $(LIBS)

# Default target: the first target is the default target.
# Just type "make -f Makefile.Linux" to build it.

all: $(PROGRAM)

rebuild: clean $(PROGRAM) clean_obj

# Link target: automatically builds its object dependencies before executing its link command.

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(LDFLAGS)

# Object targets: rules that define objects, their dependencies, and a list of commands for compilation.
# Clean target: "make -f Makefile.Linux clean" to remove unwanted objects and executables.

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)
	
clean_obj: 
	$(RM) -f $(CXXOBJECTS)

# Run target: "make -f Makefile.Linux run" to execute the application
#             You will need to add $(VARIABLE_NAME) for any command line parameters 
#             that you defined earlier in this file.