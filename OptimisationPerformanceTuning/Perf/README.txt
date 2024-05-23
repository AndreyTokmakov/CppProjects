
https://perf.wiki.kernel.org/index.php/Main_Page

---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------


export GCC_VERSION=13.2
export GCC_PATH=/home/andtokm/DiskS/Utils/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

make

---------------------------------------------------------------------------------------------
        Install and configure
---------------------------------------------------------------------------------------------

To install perf on ubuntu one needs (this is inspired from here)

sudo apt install linux-tools-common
sudo apt install linux-tools-generic
sudo apt install linux-tools-`uname -r`



---------------------------------------------------------------------------------------------
        Permissions | https://github.com/NAThompson/performance_tuning_tutorial
---------------------------------------------------------------------------------------------

sudo echo "-1" | sudo tee /proc/sys/kernel/perf_event_paranoid

#!/bin/bash

# Taken from Milian Wolf's talk "Linux perf for Qt developers"
sudo mount -o remount,mode=755 /sys/kernel/debug
sudo mount -o remount,mode=755 /sys/kernel/debug/tracing
echo "0" | sudo tee /proc/sys/kernel/kptr_restrict
echo "-1" | sudo tee /proc/sys/kernel/perf_event_paranoid
sudo chown `whoami` /sys/kernel/debug/tracing/uprobe_events
sudo chmod a+rw /sys/kernel/debug/tracing/uprobe_events

---------------------------------------------------------------------------------------------
        How to run | Simple example
---------------------------------------------------------------------------------------------

echo 0 > /proc/sys/kernel/nmi_watchdog
echo "0" | sudo tee /proc/sys/kernel/kptr_restrict
echo "-1" | sudo tee /proc/sys/kernel/perf_event_paranoid

    perf stat ls
    perf stat sleep 1

     perf stat ls
    time perf stat sleep 1

echo "2" | sudo tee /proc/sys/kernel/perf_event_paranoid
echo "1" | sudo tee /proc/sys/kernel/kptr_restrict
echo 1 > /proc/sys/kernel/nmi_watchdog

---------------------------------------------------------------------------------------------
        Example
---------------------------------------------------------------------------------------------

    > time perf stat ./test_app

    Running test app
    110000

     Performance counter stats for './test_app':

              1,263.90 msec task-clock                #    0.184 CPUs utilized
               110,021      context-switches          #   87.049 K/sec
                   556      cpu-migrations            #  439.910 /sec
                   125      page-faults               #   98.901 /sec
           945,153,253      cycles                    #    0.748 GHz
            68,591,411      stalled-cycles-frontend   #    7.26% frontend cycles idle
             6,014,905      stalled-cycles-backend    #    0.64% backend cycles idle
           943,328,798      instructions              #    1.00  insn per cycle
                                                      #    0.07  stalled cycles per insn
           236,537,566      branches                  #  187.150 M/sec
             5,931,090      branch-misses             #    2.51% of all branches

           6.870878032 seconds time elapsed

           0.102010000 seconds user
           1.084531000 seconds sys



    real	0m6.922s
    user	0m0.111s
    sys	0m1.093s

---------------------------------------------------------------------------------------------
        Example + L1 Caches stats
---------------------------------------------------------------------------------------------

> time perf stat -d ./test_app

          1,427.39 msec task-clock                #    0.201 CPUs utilized
           110,015      context-switches          #   77.074 K/sec
                85      cpu-migrations            #   59.549 /sec
               128      page-faults               #   89.674 /sec
       998,052,592      cycles                    #    0.699 GHz                      (60.48%)
        67,952,269      stalled-cycles-frontend   #    6.81% frontend cycles idle     (68.37%)
         6,171,436      stalled-cycles-backend    #    0.62% backend cycles idle      (80.51%)
       934,911,956      instructions              #    0.94  insn per cycle
                                                  #    0.07  stalled cycles per insn  (83.48%)
       233,501,692      branches                  #  163.586 M/sec                    (74.80%)
         5,397,731      branch-misses             #    2.31% of all branches          (71.43%)
       346,764,087      L1-dcache-loads           #  242.936 M/sec                    (84.21%)
        16,234,732      L1-dcache-load-misses     #    4.68% of all L1-dcache accesses  (76.72%)
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses

       7.084458621 seconds time elapsed

       0.092412000 seconds user
       1.232163000 seconds sys


> time perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores -d ./test_app


       237,006,030      L1-dcache-loads
         5,536,627      L1-dcache-load-misses     #    2.31% of all L1-dcache accesses
   <not supported>      L1-dcache-stores
       242,952,356      L1-dcache-loads
         5,545,357      L1-dcache-load-misses     #    2.31% of all L1-dcache accesses
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses

       6.690696409 seconds time elapsed

       0.053749000 seconds user
       0.920457000 seconds sys


---------------------------------------------------------------------------------------------
        Reporting
---------------------------------------------------------------------------------------------

>   perf record -g ./test_app
>   perf report -g

    # As the result the 'perf.data' file will be created use 'perf report' to anylize recording

>   perf report

>   perf report --sort=dso

---------------------------------------------------------------------------------------------
        System-Wide Real-Time Performance Counter Profile
---------------------------------------------------------------------------------------------

>   sudo perf top

    # Monitoring the system-wide performance counters in real-time helps in identifying the
    # most resource-intensive components of the system. It provides a quick overview of the
    # system’s behavior and areas that may require optimization.

---------------------------------------------------------------------------------------------
        Recording Process Profiles
---------------------------------------------------------------------------------------------

>   sudo perf record -p PID

    # Recording process profiles is useful when analyzing the performance of a long-running
    # process or debugging a specific issue in real-time. It allows us to record and inspect the
    # performance profile without interrupting the process.
