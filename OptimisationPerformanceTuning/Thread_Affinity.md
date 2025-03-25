### Thread Affinity

Thread affinity refers to binding a thread to a specific CPU core or set of cores, ensuring it consistently runs on that core.</br>
Without explicit affinity, the operating system might schedule threads to run on different cores at various times,</br>
causing frequent cache invalidations and migrations, which negatively impact performance.</br>

<b><u>When threads migrate between cores:</b></u></br>
1. Cache locality is reduced because the thread loses cached data, increasing cache misses and latency. 
2. Cache coherency traffic increases, leading to performance degradation due to additional overhead.

<b><u>Setting thread affinity helps achieve:</b></u></br>
1. Better cache locality: Data stays warm in the local cache. 
2. Reduced context-switch overhead: Threads experience fewer stalls, leading to smoother execution. 
3. Improved predictability and stability: Threads’ performance becomes more predictable, essential for real-time or latency-sensitive applications.

<b><u>How to (Linux):</b></u></br>
```
In C++, you can set thread affinity using OS-specific APIs:
#include <pthread.h>
#include <sched.h>

void setAffinity(std::thread &thread, int core_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
}
```