
### Function Inlining and Instruction Cache Effects

Frequent function calls can add overhead — not just the call/return instructions, but also potential instruction
cache misses if the function’s code is far away in memory.<br>
Function inlining is an optimization where the compiler replaces a function call with the function’s body
(as if you wrote it inline).<br>
This saves the call overhead and can also enable further optimizations across the function boundary.


### Inlining benefits:

1. Eliminating call overhead:
   A normal function call involves jumping to another location (which may disrupt instruction flow), pushing return addresses, etc.
   Inline expansion avoids that.<br>
   For very small functions (think getters, setters, tiny arithmetic functions), this can save a few cycles and improve performance

2. Enabling further optimizations: Once inlined, the function’s code is part of the caller, so the compiler might optimize it more.<br>
   It can const-propagate values, remove dead code, etc., across what used to be a call boundary.<br>
   Inlining can also help with auto-vectorization or unrolling if the function was called inside a loop

3. Smaller code (in some cases):
   If a function is only called once, inlining it will remove the need to keep a separate copy of its code
   and eliminate call setup, potentially making the program smaller
   (However, the opposite is usually true when a function is called many times; see below.).

### Inlining drawbacks:

1. Code bloat:<br>
   Inlining replicates the function’s code at each call site.<br>
   If you inline a medium-sized function that’s called from 10 places, you now have 10 copies of that code.
   This increases the binary size and can have negative effects on the instruction cache.<br>
   The CPU’s instruction cache (and decode pipelines) have limited capacity, and if your hot code doesn’t fit, it can slow down.<br>
   Thus, inlining is a trade-off: speed from avoiding calls versus potential slowdowns from a larger code footprint<br><br>

2. The compiler might refuse to inline a function if it thinks the cost outweighs the benefit
   (or if the function is recursive/virtual/etc.).<br>
   The inline keyword in C++ is more about linkage than a command to inline, though compilers do take it as a hint.

   Use [[gnu::always_inline]] (GCC/Clang) or __forceinline (MSVC) if you really need to force it,
   but that’s rarely needed except in critical low-level code.

### Tips:

1. Inline small, frequently-called functions, especially those in tight loops.<br>
   Good candidates are simple getters/setters or small math functions.<br>
   Modern C++ standard library uses inline aggressively for things like std::vector::size()
   (which is typically just returning a member variable).

2. Be cautious about inlining large functions or those with complex logic, as it can blow up your code size.<br>
   Measure the impact — sometimes keeping a function call (which might be predicted well by the CPU’s return predictor) is fine.

3. Remember that LTO (Link Time Optimization), discussed later, can inline even across source files.<br>
   So you don’t have to put everything in headers as inline functions to get cross-module inlining – LTO can allow inlining
   of non-header functions as well, if enabled.