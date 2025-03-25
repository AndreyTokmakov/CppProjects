### Avoiding False Sharing

False sharing is a sneaky performance bug in multithreading.</br>
It occurs when threads are technically working on different variables, but those variables happen to reside on the same cache line.
Each core has its own cache, and when one core updates a cache line, it invalidates that line in other cores’ caches
(per the cache-coherency protocol).</br>
If two threads keep ping-ponging updates to the same cache line (even if to different data on that line), 
they cause a lot of coherency traffic and stalls.</br>
For example, suppose you have:

```                                                    
struct Counters {                              void thread1() {
    std::atomic<int> c1;                           for (...) counters.c1.fetch_add(1);
    std::atomic<int> c2;                       }
} counters;                                    void thread2() { 
                                                   for (...) counters.c2.fetch_add(1);  
                                               }
```


If `c1` and `c2` end up on the same cache line (likely, since they are adjacent in memory in the struct), `thread1` and `thread2` will</br> 
interfere with each other’s caches even though they work on different atomic variables.</br>
Each increment invalidates the line for the other core, causing them to continually reload it.</br>
This false sharing can dramatically slow down throughput – sometimes by an order of magnitude, as the CPUs spend 
time passing the cache line back and forth</br>


<u><b>Solution:</b></u></br>
Padding or aligning to separate such variables. We can prevent `c1` and `c2` from sharing a cache line:
```
struct PaddedCounters
{
    alignas(64) std::atomic<int> c1;
    alignas(64) std::atomic<int> c2;
};
```

Given a typical cache line is 64 bytes, alignas(64) ensures each counter starts on its own cache line.</br>
The compiler may also insert padding between them because of the alignment, so they won’t reside on the same line.</br>
Now thread1 and thread2 operate on independent cache lines, and there’s no false sharing.</br>
The memory cost is a few dozen extra bytes, which is negligible compared to the performance gain in a hot multi-threaded loop.</br>