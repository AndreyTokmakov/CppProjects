============================================================================================

> clang++-17 demo1.cpp -o demo1.o -std=c++20 -Xclang -fdump-record-layouts -D DUMP_RECORD_LAYOUTS

============================================================================================

*** Dumping AST Record Layout
         0 | struct A
         0 |   uint64_t data_of_A
           | [sizeof=8, dsize=8, align=8,
           |   nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | struct B
         0 |   struct A (base)
         0 |     uint64_t data_of_A
         8 |   uint64_t data_of_B
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]


============================================================================================

> clang++-17 demo2.cpp -o demo2.o -std=c++20 -Xclang -fdump-record-layouts -D DUMP_RECORD_LAYOUTS

============================================================================================

*** Dumping AST Record Layout
         0 | struct A
         0 |   uint64_t data_of_A
           | [sizeof=8, dsize=8, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | struct B
         0 |   struct A (base)
         0 |     uint64_t data_of_A
         8 |   uint64_t data_of_B
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*** Dumping AST Record Layout
         0 | struct C
         0 |   struct B (base)
         0 |     struct A (base)
         0 |       uint64_t data_of_A
         8 |     uint64_t data_of_B
        16 |   uint64_t data_of_C
           | [sizeof=24, dsize=24, align=8,
           |  nvsize=24, nvalign=8]
