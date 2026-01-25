# 🧵 Потенциальные вопросы на собеседовании (C++ Multithreading)

---

## 🟢 База / разогрев
- Что такое **thread-safety**?  [ответ](#what_is_thread_safety)
- Чем **concurrency** отличается от **parallelism**?
- Что такое **critical section**?
- Когда `std::thread` дешевле, чем `std::async`, и наоборот? [ответ](#when_to_choose_thread_vs_async)
- Что происходит при исключении внутри `std::thread`?
- Можно ли копировать `std::thread`? Почему?  [ответ](#can_std_thread_be_copied)
- В чём разница между `detach()` и `join()`?
- Почему `sleep_for` — плохая синхронизация?
- ожно ли безопасно завершить detached thread?
- Что произойдёт, если два потока пишут в `std::vector`?

---

## 🟡 Mutex / Locks
- Чем `std::mutex` отличается от `std::recursive_mutex`?
- Когда `recursive_mutex` — плохая идея?
- Что такое **lock convoy**? [ответ](#lock_convoy)  
- Зачем нужен `std::try_lock`?
- Можно ли захватить mutex в signal handler? [ответ](#signal_handler)
- Можно ли использовать один mutex для разных данных?
- Почему long-held mutex — антипаттерн?
- Чем `std::scoped_lock` лучше `std::lock_guard`?
- Что такое priority inversion? [ответ](#priority_inversion)
- Как priority inversion решается?

---

## 🟠 Condition Variables / Coordination
- Почему `condition_variable` не хранит состояние?
- Что произойдёт, если вызвать `notify_one` без mutex?
- Почему `notify_all` может быть опасен?
- Что будет, если `notify` вызван до `wait`?
- Можно ли использовать `condition_variable` без mutex?
- Чем `condition_variable_any` хуже?
- Когда лучше использовать `latch` или `barrier`?
- Можно ли реализовать semaphore через `condition_variable`?

---

## 🟠 Atomics / Memory Model
- В чём разница между **atomicity** и **visibility**?
- Почему `std::atomic<T>` не всегда lock-free?
- Что значит `is_lock_free()`?
- Когда `compare_exchange_weak` предпочтительнее `strong`?
- Может ли `atomic<int>` привести к cache line bouncing?
- Почему atomic может быть медленнее mutex?
- В чём разница между acquire/release и seq_cst?
- Можно ли смешивать разные memory_order?
- Что такое **out-of-thin-air reads**?
- Почему UB из-за data race хуже segfault?
- Почему CAS может спуриться (fail spuriously)?
- Что такое release sequence?

---
## 🔴 Продвинутые темы
- Что такое **ABA problem** и где она возникает?
- Почему lock-free ≠ fast?
- Что такое **wait-free progress guarantee**?
- Что сложнее: MPMC queue или SPSC queue?
- Почему memory reclamation — самая сложная часть lock-free?
- Что такое hazard pointers?
- Чем RCU отличается от mutex?
- Что такое quiescent state?
- Почему lock-free код сложнее тестировать?
- Можно ли сделать lock-free `std::vector`?
- `Data race` vs `Race condition`
- `lock-free` vs `wait-free`

---

## 🔥 Производительность / Архитектура
- Что такое cache coherence?
- MESI протокол — кратко
- Почему false sharing опаснее, чем contention?
- Что такое false sharing и как mutex может его усугублять?
- или Когда spinlock лучше mutex’а?
- Что такое NUMA?
- Как NUMA влияет на mutex?
- Когда pinning потоков полезен?
- Почему `malloc/free` плохо масштабируются?
- Как аллокаторы влияют на multithreading?
- Почему shared_ptr дорогой в MT?
- Как измерить contention?

---

## 🧪 Debugging / Production
- Как найти data race в проде?
- Почему race может исчезнуть при логировании?
- Как ThreadSanitizer влияет на memory ordering?
- Можно ли полагаться на тесты для MT кода?
- Почему MT баги не воспроизводятся?
- Как писать unit-тесты для concurrent кода?
- Как безопасно останавливать сервис с потоками?
- Что происходит при `fork()` в многопоточном процессе?
- Что такое async-signal-safe?
- Можно ли ловить deadlock автоматически?

---

## 🧠 Вопросы «на мышление»
- Как бы ты объяснил memory model джуниору?
- Что ты выберешь: проще код или быстрее?
- Когда лучше отказаться от lock-free?
- Какие MT баги ты ловил лично?
- Как бы ты спроектировал очередь для HFT?
- Как защитить read-mostly структуру?
- Как масштабировать логгер на 100+ потоков?
- Что опаснее: race или deadlock — и почему?
- Какой самый сложный concurrent баг ты видел?
- Какие MT темы ты считаешь «красным флагом»?

---

<a name="what_is_thread_safety"></a>
### 🟢 Что такое thread-safety?

Thread-safety — это свойство кода или объекта корректно работать при одновременном доступе из нескольких потоков,
без data race и UB, сохраняя свои инварианты.


<a name="when_to_choose_thread_vs_async"></a>
### 🟢 Когда std::thread дешевле, чем std::async, и наоборот?

std::thread дешевле и предсказуемее,
std::async удобнее, но может быть дороже и менее контролируем.

Когда std::thread дешевле
1️Нужен полный контроль над потоком
    время старта
    lifetime
    affinity
    приоритет

std::async:
    может создать поток
    может использовать thread pool
    может вообще выполниться синхронно (!)


| Критерий          | std::thread | std::async       |
| ----------------- | ----------- | ---------------- |
| Создание потока   | всегда      | не гарантировано |
| Контроль          | полный      | ограниченный     |
| Возврат значения  | вручную     | встроен          |
| Исключения        | теряются    | передаются       |
| Накладные расходы | минимальные | выше             |
| Подходит для HFT  | ✔           | ❌                |
| Boilerplate       | больше      | меньше           |


<a name="can_std_thread_be_copied"></a>
### 🟢 Можно ли копировать std::thread? Почему?

	Нет, std::thread нельзя копировать.
	Его можно только перемещать.

#### Что разрешено

```cpp
std::thread t1(worker);
std::thread t2 = std::move(t1);  // OK
```

После `move`:

* `t1.joinable() == false`
* `t2` владеет потоком

#### Что запрещено

```cpp
std::thread t1(worker);
std::thread t2 = t1;   // ❌ compile error
```

<a name="lock_convoy"></a>
### 🟢 Что такое lock convoy?

Lock convoy — это ситуация, когда несколько потоков одновременно пытаются захватить один mutex,
и каждый поток вынужден ждать, что вызывает длительные задержки и деградацию производительности.

Проще: очередь потоков за mutex, где каждый поток блокируется и пропускает CPU, создавая “конвой” потоков.

Lock convoy — это цепочка потоков, блокирующихся на одном mutex, когда каждый ждёт своего хода, что вызывает задержки и падение производительности.
Проблему решают уменьшением критических секций, использованием нескольких mutex или lock-free структур.

<a name="signal_handler"></a>
### 🟢 Можно ли захватить mutex в signal handler?

**Нет**. Mutex нельзя захватывать в signal handler, потому что обработчик может прервать поток, уже владеющий mutex, а сами mutex’ы не async-signal-safe.
Это приводит к deadlock или UB.
Signal handler может прервать **любой поток**, в том числе **тот, который уже держит этот mutex**:

```cpp
void handler(int) {
    m.lock();   // 💥 если mutex уже захвачен — deadlock
}

```
#### Signal handler ≠ обычный код
В обработчике сигнала:
* нельзя аллоцировать память
* нельзя логировать
* нельзя блокироваться
* нельзя трогать мьютексы, condition variables, STL

#### Что вообще можно в signal handler?
По POSIX — **очень ограниченный список**, например:
* `_exit`
* `write`
* `signal`
* операции с `sig_atomic_t`

#### ❌ Типичный плохой код

```cpp
std::mutex m;

void handler(int) {
    std::lock_guard lg(m); // ❌ UB
    shutdown();
}
```

#### ✅ Как **правильно** делать
Atomic flag (самый частый ответ на интервью)

```cpp
std::atomic<bool> stop{false};

void handler(int) {
    stop.store(true, std::memory_order_relaxed);
}
```

А в рабочем коде:

```cpp
while (!stop.load()) {
    do_work();
}
```

✔ async-signal-safe
✔ без блокировок

<a name="priority_inversion"></a>
### 🟢 Что такое priority inversion?

**Priority inversion** — это ситуация, когда **высокоприоритетный поток блокируется низкоприоритетным**, а среднеприоритетные потоки не дают низкому освободить ресурс.

Priority inversion — это когда high-priority поток блокируется low-priority из-за общего ресурса, а средние приоритеты не дают low его освободить; решается priority inheritance.

**Почему плохо:**

* непредсказуемая latency
* starvation
* нарушение real-time гарантий

**Как решают:**

* **Priority inheritance** (лучший ответ)
* Priority ceiling
* Короткие critical sections
* Lock-free подходы

**Что НЕ помогает:**

* spinlock
* `sleep_for`
* повышение приоритета H
