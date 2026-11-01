# ThreadSafePriorityQueue

A thread-safe priority queue implemented in C++20 using `std::deque`, `std::mutex`, and `std::condition_variable`.

The queue supports multiple producers and multiple consumers, three priority levels, configurable capacity limits, and graceful shutdown.

## Overview

`ThreadSafePriorityQueue<T>` is a multi-producer, multi-consumer (MPMC) queue designed for concurrent task processing.

It maintains three independent FIFO queues:

- **High** — highest priority.
- **Medium** — medium priority.
- **Low** — lowest priority.

Consumers always retrieve tasks from the highest-priority non-empty queue.

The implementation uses a single mutex to protect the internal state and condition variables to coordinate producers and consumers.

## Features

- Thread-safe concurrent access.
- Three priority levels: High, Medium, and Low.
- Strict priority scheduling.
- FIFO ordering within each priority level.
- Configurable capacity for High and Medium queues.
- Unlimited capacity for Low queue.
- Blocking producers when bounded queues are full.
- Blocking consumers when all queues are empty.
- Graceful shutdown with `close()`.
- No external dependencies.

## Architecture

The queue maintains three independent internal queues:

```text
ThreadSafePriorityQueue
|
+-- High Queue
|   +-- FIFO
|   +-- Bounded capacity
|
+-- Medium Queue
|   +-- FIFO
|   +-- Bounded capacity
|
+-- Low Queue
    +-- FIFO
    +-- Unlimited capacity
```

Each internal queue is implemented using `std::deque<T>`.

A single `std::mutex` protects all internal queues and the queue's shutdown state.

Two types of condition variables are used:

- `notEmpty` — notifies consumers when new tasks become available.
- `notFull` — notifies producers when capacity becomes available in a bounded queue.

## Priority Scheduling

The queue implements strict priority scheduling.

The priority order is:

```text
High -> Medium -> Low
```

When a consumer requests a task, the queue checks the priority levels in order and retrieves the first available task.

For example, consider the following tasks:

```text
High:   H1, H2
Medium: M1, M2
Low:    L1, L2
```

The retrieval order will be:

```text
H1, H2, M1, M2, L1, L2
```

FIFO ordering is preserved within each priority level.

### Priority Starvation

Since the queue uses strict priority scheduling, lower-priority tasks may experience starvation if higher-priority queues continuously receive new tasks.

This behavior is intentional and follows the scheduling policy.

## Capacity Management

The constructor accepts two capacity parameters:

```cpp
ThreadSafePriorityQueue(std::size_t highCapacity,
                       std::size_t mediumCapacity);
```

The capacity rules are:

| Priority | Capacity | Producer behavior |
|---|---|---|
| High | Configurable | Blocks when full |
| Medium | Configurable | Blocks when full |
| Low | Unlimited | Does not block due to capacity |

Both High and Medium capacities must be greater than zero.

The Low queue has no configured capacity limit.

### Producer Blocking

When a producer attempts to add a task to a full High or Medium queue, it waits until capacity becomes available.

When a consumer removes a task from a bounded queue, a waiting producer may resume.

Producers adding tasks to other queues are not blocked by the capacity of a different priority level.

## API

### Constructor

```cpp
ThreadSafePriorityQueue(std::size_t highCapacity,
                       std::size_t mediumCapacity);
```

Creates a queue with the specified capacities.

Throws `std::invalid_argument` if either capacity is zero.

### push()

```cpp
bool push(T value, Priority priority);
```

Adds a task to the specified priority queue.

Behavior:

- Adds the task if the queue is open and capacity is available.
- Blocks if the selected bounded queue is full.
- Returns `false` if the queue has been closed.
- Throws `std::invalid_argument` if the priority value is invalid.

### waitAndPop()

```cpp
std::optional<T> waitAndPop();
```

Retrieves the highest-priority available task.

Behavior:

- Blocks if all queues are empty and the queue is open.
- Returns a task when one becomes available.
- Returns `std::nullopt` when the queue is closed and all queues are empty.

### close()

```cpp
void close();
```

Initiates graceful shutdown.

After calling `close()`:

- New tasks are rejected.
- Blocked producers are awakened.
- Blocked consumers are awakened.
- Existing tasks remain available for processing.

Consumers can continue retrieving tasks until all internal queues are empty.

Subsequent calls to `waitAndPop()` return `std::nullopt`.

### empty()

```cpp
bool empty() const;
```

Returns `true` if all internal queues are empty.

### size()

```cpp
std::size_t size(Priority priority) const;
```

Returns the number of tasks currently stored in the specified priority queue.

### totalSize()

```cpp
std::size_t totalSize() const;
```

Returns the total number of tasks across all priority queues.

## Usage Example

```cpp
#include "ThreadSafePriorityQueue.hpp"

#include <iostream>
#include <thread>

int main()
{
    using Queue = priority_queue::ThreadSafePriorityQueue<int>;
    using Priority = priority_queue::Priority;

    Queue queue(100, 200);

    std::thread consumer([&queue] {
        while (auto task = queue.waitAndPop()) {
            std::cout << "Processing task: " << *task << '\n';
        }
    });

    queue.push(1, Priority::Low);
    queue.push(2, Priority::Medium);
    queue.push(3, Priority::High);

    queue.close();

    consumer.join();

    return 0;
}
```

Expected output:

```text
Processing task: 3
Processing task: 2
Processing task: 1
```

## Thread Safety

All public operations are thread-safe.

Multiple producers can concurrently call `push()`.

Multiple consumers can concurrently call `waitAndPop()`.

The internal mutex ensures that queue modifications and task retrieval are synchronized.

Condition variables are used to avoid busy waiting.

## Shutdown Behavior

The queue supports graceful shutdown.

A typical shutdown sequence is:

1. Stop submitting new tasks.
2. Call `close()`.
3. Allow consumers to process remaining tasks.
4. Consumers exit when `waitAndPop()` returns `std::nullopt`.
5. Join all producer and consumer threads.
6. Destroy the queue.

The queue must not be destroyed while other threads are still accessing it.

## Implementation Details

### Internal Data Structures

Each priority queue contains:

- `std::deque<T>` — storage for tasks.
- `std::size_t maxCapacity` — maximum number of tasks.
- `std::condition_variable notFull` — notification mechanism for blocked producers.

The main queue contains:

- `std::array<PriorityQueue, 3>` — internal priority queues.
- `std::mutex` — synchronization.
- `std::condition_variable notEmpty` — consumer notification.
- `bool closed` — shutdown state.

### Complexity

Let `P` be the number of priority levels.

Since the implementation uses three fixed priority levels:

| Operation | Complexity |
|---|---|
| `push()` | O(1) amortized |
| `waitAndPop()` | O(P), effectively O(1) |
| `empty()` | O(P), effectively O(1) |
| `size()` | O(1) |
| `totalSize()` | O(P), effectively O(1) |

The complexity of `push()` and `waitAndPop()` excludes time spent waiting for synchronization or queue capacity.

## Limitations

- Strict priority scheduling may cause starvation of lower-priority tasks.
- Low queue capacity is unlimited, so its memory usage may grow without bound.
- A single mutex serializes access to all internal queues.
- The queue does not provide cancellation of individual tasks.
- The queue does not guarantee FIFO ordering between producers waiting for capacity.
- The queue does not automatically manage the lifetime of producer or consumer threads.

## Requirements

- C++20 compatible compiler.
- Standard C++ library with threading support.
- No external dependencies.