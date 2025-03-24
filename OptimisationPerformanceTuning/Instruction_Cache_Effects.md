# instruction:

The instruction cache (L1i) is usually around 32KB on many CPUs (and a micro-op cache on Intel decoders as well). 
This cache holds the instructions your program is executing. 
If you have a lot of code concentrated in a hot path (due to inlining and unrolling, for instance), 
you could overflow the cache and incur instruction fetch stalls.

It’s a nuanced point: more inlining = fewer calls, but also = more code bytes. 
As an expert, you should be aware of this balance. 
Sometimes, less is more: it could be beneficial to not inline certain functions to keep the tight loop smaller, 
fitting in the uop cache or L1i. 

This is highly microarchitecture-dependent, so it’s an area to use profiling and perhaps processor
performance counters (like ITLB misses or L1i misses) if you suspect instruction cache issues.