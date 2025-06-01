
make clean all

valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --branch-sim=yes --collect-jumps=yes ./app