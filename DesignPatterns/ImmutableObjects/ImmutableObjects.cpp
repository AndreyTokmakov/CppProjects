/**============================================================================
Name        : ImmutableObjects.cpp
Created on  : 13.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ImmutableObjects.cpp
============================================================================**/

#include "ImmutableObjects.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <syncstream>

/*
 * Immutable Objects / Snapshot Publication
 *
 * Demonstrates a thread-safe mechanism for publishing and accessing
 * immutable object snapshots using std::atomic<std::shared_ptr<T>>.
 *
 * Architecture:
 *
 *   Scheduler Thread                  Dumper Thread
 *         |                                |
 *         | updateConfiguration()          | dump()
 *         |                                |
 *         v                                v
 *   Create new version              Capture current version
 *         |                                |
 *         v                                v
 *   Atomic publication  ----------> shared_ptr<const Configuration>
 *         |                                |
 *         |                                v
 *         |                         Simulate slow I/O
 *         |                                |
 *         v                                v
 *   Publish newer versions          Serialize captured version
 *
 *
 * Design principles:
 *
 * 1. Immutable Objects
 *    Published Configuration objects are never modified.
 *    Every update creates a new Configuration instance.
 *
 * 2. Atomic Publication
 *    std::atomic<std::shared_ptr<const Configuration>> provides
 *    atomic publication and acquisition of Configuration versions.
 *
 * 3. Lifetime Management
 *    Each reader holds its own shared_ptr to the captured version.
 *    Publishing a newer version does not invalidate existing readers.
 *    An obsolete Configuration is destroyed only after its last
 *    shared_ptr reference is released.
 *
 * 4. Non-blocking Reader/Writer Interaction
 *    Scheduler does not wait for Dumper to finish reading or serializing
 *    an older Configuration before publishing a newer version.
 *
 *    Dumper does not need to lock Scheduler while accessing a captured
 *    Configuration.
 *
 * 5. Memory Visibility
 *    The writer publishes objects using memory_order_release.
 *    Readers acquire published objects using memory_order_acquire.
 *    This guarantees visibility of the fully initialized Configuration
 *    to readers that acquire the published pointer.
 *
 *
 * Guarantees:
 *
 * - A reader always accesses a fully constructed Configuration.
 * - A captured Configuration remains alive for the lifetime of its
 *   shared_ptr, regardless of subsequent publications.
 * - Multiple readers can safely access the same immutable version.
 * - Publishing a new version does not modify previously published versions.
 * - No mutex is explicitly used for Configuration publication or reading.
 *
 *
 * Limitations:
 *
 * - std::atomic<std::shared_ptr<T>> is not guaranteed to be lock-free.
 * - Creating a new Configuration requires dynamic memory allocation.
 * - Reference counting introduces atomic operations.
 * - Destruction of an obsolete Configuration can occur in the thread
 *   that releases the last shared_ptr reference.
 * - The version counter assumes a single writer thread.
 * - This design provides per-object consistency, not a globally
 *   consistent snapshot across multiple independent modules.
 *
 *
 * Typical use cases:
 *
 * - Runtime configuration updates.
 * - Immutable application state publication.
 * - Background serialization and persistence.
 * - Snapshot creation without blocking application threads.
 *
 * The approach is particularly suitable for relatively infrequent
 * updates of complex objects that may be read or serialized concurrently.
 *
 * For frequently updated Hot Path state, alternatives such as atomic
 * variables, RCU, or Triple Buffering may provide lower overhead.
 */


namespace immutable_objects
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr std::string_view formatMSeconds { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    [[nodiscard]]
    std::string getCurrentTime(const time_point<system_clock>& timestamp = system_clock::now())
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), formatMSeconds.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count()
        );
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "

    struct Configuration
    {
        std::uint64_t version { 0 };
        std::string name;
        std::vector<std::string> queues;
    };

    class Scheduler
    {
    public:

        Scheduler()
        {
            updateConfiguration({"QueueA", "QueueB"});
        }

        void updateConfiguration(std::vector<std::string> queues)
        {
            const uint64_t version = ++currentVersion;
            auto config = std::make_shared<const Configuration>(Configuration {
                .version = version,
                .name = "Scheduler",
                .queues = std::move(queues)
            });

            configuration.store(std::move(config), std::memory_order_release);
            LOG << "[Scheduler] Published version " << version << '\n';
        }

        [[nodiscard]]
        std::shared_ptr<const Configuration> getConfiguration() const
        {
            return configuration.load(std::memory_order_acquire);
        }

    private:

        uint64_t currentVersion { 0 };
        std::atomic<std::shared_ptr<const Configuration>> configuration {};
    };

    class Dumper
    {
    public:

        explicit Dumper(const Scheduler& scheduler): scheduler(scheduler)
        {
        }

        void dump() const
        {
            // Capture the currently published Configuration.
            // The local shared_ptr holds a reference to this exact version.
            // Subsequent Scheduler updates cannot change the captured object.
            const std::shared_ptr<const Configuration> snapshot = scheduler.getConfiguration();

            LOG << "[Dumper   ] Starting dump, version " << snapshot->version << '\n';

            // Simulate slow disk operation.
            std::this_thread::sleep_for(2s);

            LOG << "[Dumper   ] Finished dump, version "<< snapshot->version << '\n';
            LOG << "[Dumper   ] Queues: ";

            // Safe to access without additional synchronization:
            // Configuration is immutable and its lifetime is guaranteed
            // by the local shared_ptr.
            for (const auto& queue : snapshot->queues) {
                std::cout << queue << ' ';
            }
            LOG << "\n";
        }

    private:

        const Scheduler& scheduler;
    };

    void demo()
    {
        Scheduler scheduler;
        Dumper dumper(scheduler);

        std::jthread schedulerThread([&scheduler]
        {
            std::this_thread::sleep_for(500ms);
            scheduler.updateConfiguration({ "QueueA", "QueueB", "QueueC" });

            std::this_thread::sleep_for(500ms);
            scheduler.updateConfiguration({ "QueueA", "QueueD" });

            std::this_thread::sleep_for(500ms);
            scheduler.updateConfiguration({ "QueueX", "QueueY", "QueueZ" });
        });

        std::jthread dumperThread([&dumper]
        {
            dumper.dump();
            std::this_thread::sleep_for(500ms);
            dumper.dump();
        });
    }
}

void immutable_objects::TestAll()
{
    immutable_objects::demo();
}

/*
2026-11-13 18:15:49.027695 [Scheduler] Published version 1
2026-11-13 18:15:49.027840 [Dumper   ] Starting dump, version 1
2026-11-13 18:15:49.527940 [Scheduler] Published version 2
2026-11-13 18:15:50.028492 [Scheduler] Published version 3
2026-11-13 18:15:50.529463 [Scheduler] Published version 4
2026-11-13 18:15:51.027950 [Dumper   ] Finished dump, version 1
2026-11-13 18:15:51.027979 [Dumper   ] Queues: QueueA QueueB 2026-11-13 18:15:51.027987
2026-11-13 18:15:51.528117 [Dumper   ] Starting dump, version 4
2026-11-13 18:15:53.528231 [Dumper   ] Finished dump, version 4
2026-11-13 18:15:53.528268 [Dumper   ] Queues: QueueX QueueY QueueZ 2026-11-13 18:15:53.528273
*/
