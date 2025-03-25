### Multithreading optimizations

1. <u>Reduce contention:</u></br>
   If you must use locks, try to minimize how long they’re held.<br>
   Use finer-grained locks if possible (lock splitting). <br>
   Or use atomic techniques to reduce how often locks are needed (e.g., atomic flags for quick checks). 
2. Read-write locks if appropriate (many readers, few writers). 
3. <u>Parallel algorithms and workload distribution</u></br>
   Ensure your threads are doing roughly equal work to avoid one straggler delaying the whole job (load balancing).
4. Avoid **False Sharing** problem