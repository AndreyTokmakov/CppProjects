
export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64

make

=================================================================================================

# Detect unclosed file descriptors.
# --track-fds=<yes|no|all> [default: no]

>   valgrind --track-fds=yes ./unclosed_descriptors_bin
