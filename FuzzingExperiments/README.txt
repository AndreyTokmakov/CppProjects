
1. Install clang:
    >  sudo apt install clang-12 --install-suggests

2. Build command examples:

    >  clang++-12 -Wall -g -fsanitize=address,fuzzer,signed-integer-overflow main.cpp -o fuzzer

    >  ./a.out 2>&1 | grep ERROR
