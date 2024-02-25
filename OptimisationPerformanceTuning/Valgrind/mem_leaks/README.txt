
export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}

export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64

make

=================================================================================================

# Use Valgrind Memcheck to detect common memory errors in myprog.

>  valgrind ./mem_leak_binary <myargs>

# Use Valgrind Memcheck to detect memory errors and memory leaks.

>  valgrind --leak-check=yes ./mem_leak_binary
>  valgrind --leak-check=full -v ./mem_leak_binary
   valgrind --leak-check=full --show-leak-kinds=all ./hello

# Detect unclosed file descriptors.

>   valgrind --track-fds=yes ./test_binary
