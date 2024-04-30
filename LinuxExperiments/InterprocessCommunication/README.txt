


Shared Memory Readme:

# we can see the shared memory files for the four processes, their identifiers,
# and the amount of memory used by executing the following commandː

> lsof +D /dev/shm
COMMAND    PID    USER   FD   TYPE DEVICE SIZE/OFF   NODE NAME
Create  139756 andtokm  mem    REG   0,26     1028 177134 /dev/shm/__SHARED_MEMORY_OBJECT_00000001
Create  139756 andtokm    3u   REG   0,26     1028 177134 /dev/shm/__SHARED_MEMORY_OBJECT_00000001
