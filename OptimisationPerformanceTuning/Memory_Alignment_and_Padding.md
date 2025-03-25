### Memory Alignment and Padding

Memory alignment can affect performance at a low level.
Alignment means that a data object’s address is a multiple of some power of two, typically the object’s size or the size of
the bus transfers.

CPUs often fetch memory in aligned chunks (cache lines), and certain instructions require or prefer aligned operands
(e.g., SSE loads used to require 16-byte alignment, AVX 32-byte).

Moreover, alignment can help avoid crossing cache line boundaries, and as we saw, it can prevent false sharing
by aligning on cache line size.


### Key points:

- Natural alignment: 

   By default, compilers align data structures so that their members sit at addresses 
   optimal for the type (e.g., 4-byte alignment for int, 8-byte for double on 64-bit systems).<br>
   They also insert padding in structs to satisfy these alignments for each member<br><br>

-  Cache line alignment:

   Sometimes you want to align a whole structure or global variable to a 64-byte boundary (assuming 64B cache line) 
   to either optimize access or avoid false sharing.<br>
   We saw using alignas(64) on a struct or member will do this.<br>
   This is useful for frequently accessed data that you want on its own cache line.<br><br>

-  Alignment for SIMD

   If you plan to use aligned SIMD instructions (like _mm256_load_ps), you need your data to be aligned to the 
   vector width (32 bytes for AVX). You can allocate dynamic memory aligned using C++17 std::aligned_alloc 
   or third-party allocators, or overallocate and adjust the pointer. <br>
   Alternatively, use container types that allow specifying alignment.<br>
   For stack or global data, use alignas(32) on the array. For example:

       alignas(32) float vec[ EightOrMore ];

   Now vec will start at a 32-byte boundary, so _mm256_load_ps(vec) is safe. <br>
   Misaligned accesses, if done inadvertently, either incur a slight penalty or (in older SSE) could even fault, 
   so it's important when doing low-level intrinsics.<br>

-  Padding for structure size:
 
   Sometimes you might add padding at the end of a struct to make its size a multiple of 64 bytes (cache line). <br>
   This is done when you expect an array of such structs and want each to start on a new line to avoid cache
   line sharing between two elements.
   
   Example:   

       struct CacheLineSlot {
           int value;
           char _pad[60]; // pad the struct to 64 bytes
       };
       static_assert(sizeof(CacheLineSlot) == 64);

   Now an array CacheLineSlot arr[100] will have each element occupying exactly one cache line.

-  Data alignment and performance:

   Misaligned accesses (not on natural boundary) can sometimes be slower because the CPU might have to do two memory
   accesses instead of one (if data crosses a boundary).<br>
   For instance, an 8-byte doublenot aligned to 8 might straddle a cache line boundary,
   forcing two cache lines to be accessed for that one value. <br>
   Ensuring alignment avoids such penalties.

### One caution:

   Over-aligning lots of small objects can waste space. <br>
   If you align every int on 64 bytes, you’ll mostly be allocating padding.<br>
   So use cache-line alignment judiciously for cases like arrays of counters for threads, or big structures
   where the cost is negligible.<br>
   For normal structs, trust the compiler’s default alignment/padding which is usually optimal for general use.