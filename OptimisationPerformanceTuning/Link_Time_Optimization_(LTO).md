### Link Time Optimization

By default, each C++ source file is compiled in isolation, and then the linker just stitches the object files together.<br>
Link Time Optimization (`LTO`) is a mode where the compiler’s optimizer runs after all files are compiled, 
having a view of the entire program.<br>
This enables whole-program optimizations that aren’t possible when compiling file-by-file.<br>

**What LTO can do:**

1. Cross-module inlining: <br>
   If you have a function defined in one .cpp and called in another, normally the compiler can’t inline 
   it because it only sees a reference to it. <br>
   With LTO, the compiler sees the body during link time and can inline it if beneficial (even if you didn’t mark it inline).

2. Inter-procedural optimizations across files: e.g., <br>
    - constant propagation across function calls that span files, 
    - removal of unused functions (dead code elimination on a global scale), 
    - optimizing virtual calls 
    when the whole program is known (de-virtualization if it can see a class is never subclassed, etc.).

3. Better optimization decisions:<br>
   Sometimes the compiler is conservative without whole-program knowledge. <br>
   For example, it might not realize a function has a single caller, so it won’t inline or optimize it fully.<br>
   With LTO it knows and can optimize accordingly (like static linking everything as one unit).<br>


Using LTO is usually as simple as a compiler/linker flag (e.g., `-flto` for GCC/Clang, or `/GL` and `/LTCG` for MSVC). <br>
It will make your build slower and memory-hungry because the optimizer is doing more heavy lifting at link time.
But the runtime speed can improve.


### Best practices with LTO:

Ensure your build pipeline supports it (all objects must be built with LTO and the linker needs to handle it).
Use it for release builds, not necessarily for every debug build (to save compile time).<br>
Combine LTO with Profile-Guided Optimization (`PGO`) if you really want to push the limit. <br>
PGO uses runtime profiling data to inform optimizations (like branch probabilities, hot/cold functions) which, 
combined with LTO, gives the optimizer full knowledge to rearrange code and optimize hot paths.


### Whole-Program Optimization beyond LTO: 

In some cases, whole-program analysis can allow for clever things like <br>

WHOLE program de-virtualization, aggressive inlining, or even things like identifying that certain 
checks are always false/true and removing them.<br>
These are niche but illustrate that more global knowledge = better optimization opportunities.<br>

_One thing to note_: <br>
LTO can sometimes uncover bugs or ODR violations in code that wasn’t apparent before 
(because the inlining/optimization might cause different behavior or catch inconsistencies).<br>
Make sure to test thoroughly.

In summary, LTO is a powerful switch to get the compiler to optimize your C++ program as a whole rather than per file. <br>
It often improves performance by enabling cross-module inlining and analysis.<br>
If you’re aiming for maximum performance, it’s definitely worth enabling in production builds.