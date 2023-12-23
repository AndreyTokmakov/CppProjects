
export GCC_13_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1
export PATH=${GCC_13_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64

make

=================================================================================================

gcc -o ./main ./test.c -fsanitize=address


=================================================================================================
                        Output of  ./test_bin
=================================================================================================

Hello!
=================================================================
==36585==ERROR: AddressSanitizer: global-buffer-overflow on address 0x0000004043b0 at pc 0x0000004011b4 bp 0x7fffe9e59bd0 sp 0x7fffe9e59bc8
WRITE of size 4 at 0x0000004043b0 thread T0
    #0 0x4011b3 in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4011b3)
    #1 0x7fdae3cf6082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010bd in _start (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4010bd)

0x0000004043b0 is located 0 bytes after global variable 'x' defined in 'main.cpp:12:5' (0x404220) of size 400
SUMMARY: AddressSanitizer: global-buffer-overflow (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4011b3) in main
Shadow bytes around the buggy address:
  0x000000404100: f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9
  0x000000404180: f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9 f9
  0x000000404200: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404280: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404300: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x000000404380: 00 00 00 00 00 00[f9]f9 f9 f9 f9 f9 00 00 00 00
  0x000000404400: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404480: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404500: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404580: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x000000404600: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==36585==ABORTING
andtokm@AndTokmUbuntu:~/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTu