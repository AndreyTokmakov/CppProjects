# Вопросы (C++ Multithreading)

## База / разогрев
* Что такое **thread-safety**?  [ответ](#what_is_thread_safety)
* Чем **concurrency** отличается от **parallelism**?
* Что такое **critical section**?
* Когда `std::thread` дешевле, чем `std::async`, и наоборот? [ответ](#when_to_choose_thread_vs_async)
* Что происходит при исключении внутри `std::thread`?
* Можно ли копировать `std::thread`? Почему?  [ответ](#can_std_thread_be_copied)
* В чём разница между `detach()` и `join()`?
* В чём разница между `yield()` и `sleep_for()`?   [ответ](#yield_vs_sleep_for)
* Почему `sleep_for` — плохая синхронизация?
* ожно ли безопасно завершить detached thread?
* Что произойдёт, если два потока пишут в `std::vector`?
* Почему деструктор `std::thread` может вызвать `std::terminate`?
* В каких случаях `detach()` допустим в production?
* Что произойдёт, если поток выбросит исключение?
* Почему `std::jthread` безопаснее `std::thread`?
* Как корректно передать ownership ресурса в поток?
* Можно ли перезапустить `std::thread` объект?

## Mutex / Locks
- Чем `std::mutex` отличается от `std::recursive_mutex`?
- Чем `std::recursive_mutex` плох? [ответ](#why_recursive_mutex_is_bad)
- Когда `recursive_mutex` — плохая идея?
- Что такое livelock? [ответ](#livelock)
- Что такое **lock convoy**? [ответ](#lock_convoy)  
- Зачем нужен `std::try_lock`?
- Можно ли захватить mutex в signal handler? [ответ](#signal_handler)
- Можно ли использовать один mutex для разных данных?
- Почему long-held mutex — антипаттерн?
- Чем `std::scoped_lock` лучше `std::lock_guard`?
- Что такое priority inversion? [ответ](#priority_inversion)
- Как priority inversion решается?
- В чём разница между `lock_guard`, `unique_lock` и `scoped_lock`?
- Почему `try_lock` может привести к livelock?
- Когда spinlock хуже mutex?
- Можно ли уничтожить mutex, если на нём кто-то ждёт?

## Condition Variables / Coordination
* Почему `condition_variable` не хранит состояние?
* Spurious wakeups — что это и почему они существуют [ответ](#spurious_wakeups)
* Что произойдёт, если вызвать `notify_one` без mutex?
* Почему `notify_all` может быть опасен?  [ответ](#notify_all_danger) 
* Что будет, если `notify` вызван до `wait`?
* Можно ли использовать `condition_variable` без mutex?
* Чем `condition_variable_any` хуже?
* Когда лучше использовать `latch` или `barrier`? [ответ](#latch_vs_barrier)
* Можно ли реализовать semaphore через `condition_variable`?
* Lost wakeups vs spurious wakeups — в чём разница
* Semaphore vs condition_variable
* Почему `wait()` всегда вызывается в цикле?
* Что будет, если забыть predicate?
* Чем `notify_one` отличается от `notify_all` на уровне производительности?
* Почему `condition_variable` не гарантирует fairness?
* Как реализовать timed wait без race condition?

## Atomics / Memory Model
- В чём разница между **atomicity** и **visibility**?
- Почему `std::atomic<T>` не всегда lock-free?  [ответ](#lock_free_atomic)
- Что значит `is_lock_free()`?
- Когда `compare_exchange_weak` предпочтительнее `strong`?  [ответ](#compare_exchange_weak_vs_stromg)
- Может ли `atomic<int>` привести к cache line bouncing?  [ответ](#cache_line_bouncing)
- `false sharing` vs `cache line bouncing` [ответ](#cache_line_bouncing_vs_false_sharing)
- Почему atomic может быть медленнее mutex?  [ответ](#atomic_slower_than_mutex)
- В чём разница между acquire/release и seq_cst?
- Можно ли смешивать разные memory_order?
- Что такое **out-of-thin-air reads**?
- Почему UB из-за data race хуже segfault?
- Почему CAS может спуриться (fail spuriously)?
- Что такое release sequence?
- Что такое LL/SC (Load-Link / Store-Conditional) [ответ](#ll_sc)
- Можно ли реализовать mutex на atomics?
- Когда `fetch_add` быстрее CAS?
- Почему `compare_exchange_weak` может спуриться?
- Что значит `is_lock_free()` и почему ему нельзя верить?
- Может ли `std::atomic<T*>` быть lock-free, а `std::atomic<uint64_t>` — нет?

## Продвинутые темы
- Что такое **ABA problem** и где она возникает?
- Почему lock-free ≠ fast?
- Что такое **wait-free progress guarantee**?
- Что сложнее: MPMC queue или SPSC queue?
- Почему memory reclamation — самая сложная часть lock-free?
- Что такое hazard pointers?  [ответ](#hazard_pointers)
- Чем RCU отличается от mutex?
- Что такое quiescent state?
- Почему lock-free код сложнее тестировать?
- Можно ли сделать lock-free `std::vector`?
- `Data race` vs `Race condition` [ответ](#data_race_vs_race_condition)
- `lock-free` vs `wait-free` [ответ](#lock_free_vs_wait_free)
- Когда lock-free оправдан?

## Производительность / Архитектура
- Что такое cache coherence? [ответ](#cache_coherence)
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
- Что такое contention и как измерить? [ответ](#contention)
- Что такое memory fence и когда он нужен?
- Почему contention растёт нелинейно?
- Thread-per-request vs thread pool?
- Work stealing — когда полезен?
- Как безопасно останавливать сервис с потоками?
- Как протестировать код на race condition?

## Debugging / Production / System / OS
- Что делает futex и почему он быстрый? [ответ](#futex)
- Чем userspace blocking отличается от kernel blocking? [ответ](#userspace_blocking_vs_kernel_blocking)
- Как найти data race в проде?
- Почему race может исчезнуть при логировании?
- Как ThreadSanitizer влияет на memory ordering?
- Можно ли полагаться на тесты для MT кода?
- Почему MT баги не воспроизводятся?
- Как писать unit-тесты для concurrent кода?
- Как безопасно останавливать сервис с потоками?
- Что происходит при `fork()` в многопоточном процессе?
- Что такое async-signal-safe?
- Почему нельзя брать mutex в signal handler?
- Можно ли ловить deadlock автоматически?

## Bugs & UB
* Почему data race — это UB, а не просто баг?
* Почему UB хуже, чем segfault?
* Может ли UB выглядеть «стабильно»?
* Почему TSan может скрыть баги?
* Что такое out-of-thin-air reads?
* Почему double-checked locking долго был сломан?

## Коварные вопросы
* Можно ли вызвать `notify_one` без mutex?
* Можно ли читать non-atomic после atomic store?
* Может ли `atomic<int>` привести к cache line bouncing?
* Гарантирует ли `seq_cst` отсутствие UB?
* Можно ли безопасно использовать relaxed для счётчиков?
* Что будет, если `notify` вызвать до `wait`?
* Можно ли сделать thread-safe singleton без mutex?
* Почему `sleep_for` — плохая синхронизация?

## Вопросы «на мышление»
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


<a name="what_is_thread_safety"></a>
### 🟢 Что такое thread-safety?

Thread-safety — это свойство кода или объекта корректно работать при одновременном доступе из нескольких потоков,
без data race и UB, сохраняя свои инварианты.

---

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
| Подходит для HFT  | ✔           | ❌               |
| Boilerplate       | больше      | меньше           |

---

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

---

<a name="yield_vs_sleep_for"></a>
### 🟢 В чём разница между `yield()` и `sleep_for()`?

`yield()` — это **добровольная передача CPU** другим runnable-потокам **того же приоритета**.
> «Я сейчас могу работать, но если есть кто-то другой — пусть он поработает»

> *yield используется, чтобы добровольно отдать CPU другим runnable потокам, уменьшая нагрузку при активном ожидании. Он полезен в spinlock или retry-циклах для снижения contention и предотвращения livelock, но не является механизмом синхронизации и не подходит для долгого ожидания.*


🔹 Что делает `yield` на самом деле

* Поток **не блокируется**
* Поток остаётся **RUNNABLE**
* Scheduler может:

  * поставить поток в конец run queue
  * дать время другому потоку
* **Никаких гарантий**: поток может быть запущен снова сразу же
 Важно: `yield` — **hint**, а не синхронизация.

Где `yield` полезен

🔹1️⃣ В spinlock / busy wait

```cpp
while (lock.test_and_set(std::memory_order_acquire)) {
    std::this_thread::yield();
}
```

**Зачем:**

* уменьшает CPU burn
* снижает cache line bouncing
* повышает шанс, что владелец lock'а выполнится и освободит его


🔹2️⃣ При oversubscription

> Потоков больше, чем ядер

Без `yield`:

* все потоки крутятся
* scheduler thrash
* latency ↑

С `yield`:

* поток «уступает»
* система стабилизируется

🔹 3️⃣ В retry-циклах (`try_lock`, CAS)

```cpp
while (!m.try_lock()) {
    std::this_thread::yield();
}
```

* предотвращает livelock
* повышает fairness

🔹 Yield vs Sleep

| Характеристика       | yield    | sleep_for |
| -------------------- | -------- | --------- |
| Syscall              | Возможно | Да        |
| Блокирует поток      | ❌        | ✅         |
| Минимальная задержка | Да       | Нет       |
| Энергопотребление    | Высокое  | Низкое    |
| Подходит для spin    | ✅        | ❌         |

🔹 Yield vs pause (CPU instruction)

* `pause` (x86):

  * уменьшает power
  * снижает contention
  * **не отдаёт CPU**
* `yield`:

  * отдаёт CPU scheduler'у

🔹 Busy-wait: без `yield` vs с `yield`

| Аспект                        | Без `yield`                         | С `yield`                |
| ----------------------------- | ----------------------------------- | ------------------------ |
| Поведение потока              | Активно крутится                    | Добровольно уступает CPU |
| Состояние потока              | RUNNABLE                            | RUNNABLE                 |
| Использование CPU             | 🔥 100% ядра                        | ⚖️ Сниженное             |
| Scheduler                     | Thrashing (частые перепланирования) | Стабильнее               |
| Прогресс владельца lock       | Затруднён                           | Ускоряется               |
| Latency системы               | ⬆️ Растёт                           | ⬇️ Стабилизируется       |
| Cache contention              | Высокий                             | Ниже                     |
| Fairness                      | ❌ Нет                               | ⚠️ Улучшается            |
| Энергопотребление             | Высокое                             | Ниже                     |
| Подходит для oversubscription | ❌ Плохо                             | ✅ Лучше                  |


---

<a name="why_recursive_mutex_is_bad"></a>
### 🟢 Чем `std::recursive_mutex` плох?

> *`std::recursive_mutex` плох тем, что скрывает ошибки дизайна, ухудшает производительность и усложняет анализ критических секций. Он решает только self-deadlock и не защищает от deadlock между потоками, поэтому почти всегда лучше обычный mutex или переработка архитектуры.*


🔹 `std::recursive_mutex` позволяет одному и тому же потоку захватывать mutex **несколько раз**, но:

* он **маскирует проблемы дизайна**
* ухудшает производительность
* усложняет reasoning о коде
* не решает большинство реальных проблем

🔹 Основная проблема: скрывает ошибки дизайна

Если нужен recursive mutex — значит:
* функции **слишком связаны**
* нарушен принцип single responsibility
* логика захвата локов размазана по стеку

🔹 Производительность хуже

* нужно хранить:
  * owner thread id
  * recursion counter
* каждый lock/unlock дороже
* больше кода → хуже кеш

---

<a name="livelock"></a>
### 🟢 Что такое livelock?

**Livelock** — это ситуация, когда потоки **не заблокированы**, активно работают, но **прогресс не происходит**.

> Потоки «живы», крутятся, реагируют друг на друга — но система стоит.

🔹 В отличие от других “locks”

| Ситуация   | Потоки        | CPU      | Прогресс              |
| ---------- | ------------- | -------- | --------------------- |
| Deadlock   | Заблокированы | 0%       | ❌                     |
| Livelock   | Активны       | 100%     | ❌                     |
| Starvation | Активны       | variable | ❌ (для одного потока) |

> *Livelock — это ситуация, когда потоки активно выполняются и реагируют друг на друга, но не делают полезного прогресса. В отличие от deadlock, потоки не заблокированы, но система фактически стоит.*


---

<a name="lock_convoy"></a>
### 🟢 Что такое lock convoy?

Lock convoy — это ситуация, когда несколько потоков одновременно пытаются захватить один mutex,
и каждый поток вынужден ждать, что вызывает длительные задержки и деградацию производительности.

Проще: очередь потоков за mutex, где каждый поток блокируется и пропускает CPU, создавая “конвой” потоков.

Lock convoy — это цепочка потоков, блокирующихся на одном mutex, когда каждый ждёт своего хода, что вызывает задержки и падение производительности.
Проблему решают уменьшением критических секций, использованием нескольких mutex или lock-free структур.

---

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

---

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

---

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

---

<a name="spurious_wakeups"></a>
### 🟢 Spurious wakeups — что это и почему они существуют?
**Spurious wakeup** — это ситуация, когда `condition_variable::wait` возвращается **без `notify` и без выполнения условия**.<br>
Это **нормальное и допустимое поведение** по стандарту C++.

#### Почему они существуют
Из-за реализации ОС и железа
* Futex’ы
* interrupts
* race при пробуждении нескольких потоков
* упрощение fast-path

Стандарт **разрешает** такие пробуждения, чтобы:

* не усложнять реализацию
* не тормозить быстрые пути

❌ Типичная ошибка

```cpp
cv.wait(lock);   // ❌
use(data);
```

✅ Правильный паттерн

```cpp
cv.wait(lock, [&] {
    return ready;
});
```

Или эквивалентно:

```cpp
while (!ready) {
    cv.wait(lock);
}
```

---

<a name="notify_all_danger"></a>
### 🟢 Почему `notify_all` может быть опасен?
`notify_all` может вызвать **thundering herd problem**: все ожидающие потоки просыпаются, конкурируют за mutex, большинство тут же засыпает обратно → скачки latency и падение производительности.


#### Thundering herd

```cpp
cv.notify_all();
```

* просыпаются **N потоков**
* mutex может захватить **только один**
* остальные:

  * проснулись зря
  * получили context switch
  * снова ушли в `wait`

📉 Итог:

* лишние wakeups
* scheduler thrashing
* cache invalidation


❌ Типичное злоупотребление

```cpp
// producer
queue.push(x);
cv.notify_all(); // ❌ хотя нужен только один consumer
```

✅ Когда `notify_all` **нужен**

✔ глобальное событие:

* shutdown
* stop flag
* barrier
* изменение режима работы

✔ когда **условие стало истинным для всех**


🧠 Best practices

* Если нужен **один поток** → `notify_one`
* Если **все должны проснуться** → `notify_all`
* Минимизировать количество ожидающих потоков
* Разделять CV по смыслу

---

<a name="latch_vs_barrier"></a>
### 🟢 Когда лучше использовать `latch` или `barrier`?

latch используют для одноразовой синхронизации, например, инициализации. barrier — для повторяющихся фаз, когда потоки должны встречаться в одной точке на каждом шаге.


| Характеристика      | latch            | barrier                  |
| ------------------- | ---------------- | ------------------------ |
| Одноразовый         | ✔                | ❌                        |
| Многоразовый        | ❌                | ✔                        |
| Счётчик             | уменьшается до 0 | сбрасывается каждый цикл |
| Фазы                | ❌                | ✔                        |
| Completion function | ❌                | ✔                        |
| Пересоздание        | нужно            | не нужно                 |

---

<a name="lock_free_atomic"></a>
### 🟢 Почему `std::atomic<T>` не всегда lock-free?

std::atomic<T> не всегда lock-free, потому что стандарт требует атомарности, но не требует реализации без блокировок.
Если аппаратно нет подходящей инструкции или тип слишком большой/плохо выровнен, реализация использует mutex.

####  Ограничения аппаратуры

CPU умеет атомарно работать **только с некоторыми размерами и типами**:

1, 2, 4, 8 байт — часто lock-free<br>
16 байт — иногда (x86 `cmpxchg16b`)<br>
больше 16 байт — почти никогда<br>

```cpp
std::atomic<int64_t>   // обычно lock-free
std::atomic<long double> // почти всегда нет
std::atomic<MyStruct> // нет
```

####  Стандарт не требует lock-free

C++ стандарт **разрешает реализацию через mutex**, если:

* нет подходящей инструкции CPU
* тип слишком большой
* тип не trivially copyable

```cpp
static_assert(std::atomic<int>::is_always_lock_free);
```

#### Как проверить
В compile-time

```cpp
std::atomic<T>::is_always_lock_free
```

В runtime

```cpp
a.is_lock_free()
```


####  Важный подвох

**Lock-free ≠ fast**

* lock-free может:

  * крутиться в CAS-цикле
  * вызывать cache line bouncing
* mutex иногда быстрее при низкой конкуренции

---

<a name="compare_exchange_weak_vs_stromg"></a>
### 🟢 Когда compare_exchange_weak предпочтительнее strong?

`compare_exchange_weak` предпочтительнее в CAS-циклах, особенно на LL/SC архитектурах, где `strong` может эмулироваться через цикл. <br>
`strong` нужен для одноразовых проверок, где ложный отказ недопустим.

Коротко

* **`weak`** — может *ложно* вернуть `false`
* **`strong`** — не имеет spurious failure

Когда `weak` предпочтительнее

В CAS-циклах (основной кейс)

```cpp
int expected = x.load();
while (!x.compare_exchange_weak(expected, expected + 1)) {
    // expected обновлён автоматически
}
```

✔ стандартный паттерн
✔ оптимальнее на ARM / Power
✔ позволяет компилятору использовать LL/SC

Почему быстрее?

На архитектурах с **LL/SC**:

* `weak` → одна попытка
* `strong` → эмулируется циклом внутри

➡ двойной цикл = хуже


Таблица различий

|                  | weak | strong |
| ---------------- | ---- | ------ |
| Spurious failure | ✔    | ❌      |
| Для циклов       | ✔    | ❌      |
| Для single-shot  | ❌    | ✔      |
| LL/SC friendly   | ✔    | ❌      |
| Обычно быстрее   | ✔    | ❌      |

---

<a name="cache_line_bouncing"></a>
### 🟢 Может ли `atomic<int>` привести к cache line bouncing?

Да, std::atomic<int> может вызывать cache line bouncing, особенно при частых RMW-операциях из нескольких потоков <br>
Атомарность не избавляет от кеш-когерентности и может серьёзно ухудшить масштабируемость.<br>

Atomic гарантирует корректность - но не гарантирует масштабируемость.<br>

🔥 Что такое cache line bouncing
* cache line ≈ 64 байта
* атомарная запись требует **exclusive ownership**
* cache line постоянно «прыгает» между ядрами

➡ падает производительность

🧪 Пример проблемы

```cpp
std::atomic<int> counter{0};

void worker() {
    for (int i = 0; i < 1'000'000; ++i)
        counter.fetch_add(1, std::memory_order_relaxed);
}
```

Запусти на 8–16 потоках —
**производительность хуже, чем mutex**.

🧠 Почему это происходит

1. `atomic<int>` лежит в cache line
2. `fetch_add` → RMW
3. CPU:

   * invalidates cache line у других ядер
   * ждёт exclusive state
4. повторяется для каждого потока

➡ **MESI ping-pong**

🛠 Как бороться

1️ Sharding / striping

```cpp
alignas(64) std::atomic<int> counters[N];
```

каждому потоку — свой


2️Thread-local + merge

```cpp
thread_local int local;
```
3️ Padding / alignment

```cpp
struct alignas(64) Counter {
    std::atomic<int> x;
};
```

4️Reduce contention

* batching
* less frequent updates

---

<a name="cache_line_bouncing_vs_false_sharing"></a>
### 🟢 false sharing vs cache line bouncing

False sharing возникает, когда разные данные лежат в одной cache line и мешают друг другу.<br>
Cache line bouncing — когда один и тот же объект постоянно перемещается между ядрами.<br>
Оба связаны с когерентностью, но причины разные.<br>

🧠 Коротко (1 строка)

* **False sharing** — *разные данные, одна cache line*
* **Cache line bouncing** — *одни и те же данные, много ядер*

🔁 Cache Line Bouncing

Что это<br>
**Один и тот же объект** (обычно atomic)
постоянно мигрирует между ядрами.

Типичный кейс
```cpp
std::atomic<int> counter;

void worker() {
    counter.fetch_add(1);
}
```

Причина
* RMW требует exclusive state
* invalidate на других ядрах
* ping-pong между кешами

Симптомы
* плохая масштабируемость
* деградация при росте потоков

🧨 False Sharing

Что это

**Разные переменные**,
но лежат **в одной cache line**.

Типичный кейс
```cpp
struct Stats {
    int a;
    int b;
};

Stats s;

thread1() { s.a++; }
thread2() { s.b++; }
```

Причина
* CPU не различает поля
* invalidate всей линии
* даже без логического конфликта

Симптомы
* внезапно медленно
* «невинный» код


|                | Cache Line Bouncing  | False Sharing        |
| -------------- | -------------------- | -------------------- |
| Данные         | одни и те же         | разные               |
| Причина        | реальная конкуренция | физическое соседство |
| Часто с atomic | ✔                    | ❌                    |
| Часто с struct | ❌                    | ✔                    |
| Исправляется   | sharding             | padding              |
| Видимость      | очевидная            | скрытая              |

---

<a name="atomic_slower_than_mutex"></a>
### 🟢 Почему atomic может быть медленнее mutex

`std::atomic` может быть медленнее `std::mutex` из-за cache line bouncing и активного спина при высокой конкуренции. <br>
Mutex умеет усыплять потоки и снижать нагрузку на кеши, поэтому при контенции он часто масштабируется лучше.<br>

1. Cache line bouncing

Что происходит
* атомик → RMW
* нужен exclusive cache line
* MESI invalidations
* ping-pong между ядрами

```cpp
counter.fetch_add(1);
```

➡ чем больше потоков — тем хуже

2. Spin vs sleep

Atomic

* CAS-loop
* активно жжёт CPU
* не уступает ядро

3. Mutex

* при контенции:

  * усыпляет поток
  * даёт другому выполнить работу

➡ при высокой конкуренции mutex выигрывает

🧨  High contention

| Потоки | Atomic     | Mutex      |
| ------ | ---------- | ---------- |
| 1–2    | быстрее    | медленнее  |
| 8–32   | деградация | стабильнее |

🛠 Как понять, что выбрать

Atomic хорош, если:

✔ мало потоков<br>
✔ low contention<br>
✔ простой счётчик<br>
✔ короткие операции

Mutex лучше, если:

✔ высокая конкуренция<br>
✔ длинная критическая секция<br>
✔ сложная логика<br>
✔ fairness важна


---

<a name="ll_sc"></a>
### 🟢 Что такое LL/SC (Load-Link / Store-Conditional)

🧠 Определение

**LL/SC** — это **атомарная пара инструкций**, которая используется в CPU для реализации lock-free операций:

1. **LL (Load-Link)** — читаем значение из памяти и «связываем» его с регистром.
2. **SC (Store-Conditional)** — пытаемся записать новое значение **только если** никто другой не изменил память с момента LL.

* Если кто-то другой изменил память → SC **неудача**
* Если не изменил → SC **успех**

🔹 Пример логики CAS на LL/SC

```text
do {
    old = LL(addr)
    new = old + 1
} while (!SC(addr, new))
```

* Повторяем, пока SC не сработает
* Легко реализовать `atomic<int>` на ARM/PowerPC

---

<a name="hazard_pointers"></a>
### 🟢 Hazard pointers
 
**Hazard pointer** — это механизм, с помощью которого поток **«помечает» объекты, которые он читает**, чтобы другие потоки не удаляли их преждевременно.

> Другими словами: поток говорит «я использую этот объект, не трогай его».

🔹 Идея

1. Поток читает объект из lock-free структуры
2. Перед этим **заносит объект в свой hazard pointer**
3. Другие потоки, которые хотят удалить объект:

   * проверяют все hazard pointers
   * если объект **не помечен** — безопасно удалить
   * если помечен — откладывают удаление

---

<a name="data_race_vs_race_condition"></a>
### 🟢 Data race vs Race condition

🔹 Коротко

| Термин             | Определение                                                                                                                         | Последствия                                   | UB? |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------- | --- |
| **Data race**      | Одновременный доступ к одной **неатомарной** переменной из нескольких потоков, при этом хотя бы один поток пишет, без синхронизации | **Undefined Behavior (UB)** в C++             | ✅   |
| **Race condition** | Логическая гонка между потоками, когда **результат зависит от порядка выполнения**                                                  | Может вести к багам, но **не обязательно UB** | ❌   |

🔹 Пример Data Race

```cpp
int x = 0;

void thread1() { x = 1; }
void thread2() { x = 2; } // ❌ data race
```

* Нет синхронизации
* Оба потока пишут одновременно → **UB**

🔹 Пример Race Condition

```cpp
std::atomic<int> x = 0;

void thread1() { x.store(1, std::memory_order_relaxed); }
void thread2() {
    if (x.load(std::memory_order_relaxed) == 1) {
        do_something();
    }
}
```

* Атомарные операции → **нет UB**
* Но результат зависит от того, кто выполнится первым → race condition
* Может быть баг логики

---

<a name="lock_free_vs_wait_free"></a>
### 🟢 `Lock-free` vs `Wait-free`

`Lock-free` гарантирует прогресс системы, но отдельный поток может застрять (starvation).<br>
`Wait-free` гарантирует, что каждый поток завершит операцию за конечное число шагов. Wait-free сложнее в реализации и обычно медленнее.<br>

🔹 Кратко

| Термин        | Определение                                                                                             | Гарантия                                                                                | Пример                                                               |
| ------------- | ------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------- | -------------------------------------------------------------------- |
| **Lock-free** | Любой поток может завершить операцию через конечное число шагов **если остальные потоки не мешают**     | Система в целом прогрессирует, но конкретный поток может быть заблокирован (starvation) | `std::atomic::fetch_add` в CAS-цикле                                 |
| **Wait-free** | Каждый поток гарантированно завершает операцию за конечное число шагов **независимо от других потоков** | Личный прогресс каждого потока                                                          | Специальные lock-free структуры (например, bounded queue с FAAArray) |

🔹 Ключевые различия

1. **Прогресс**

   * Lock-free → гарантируется прогресс системы
   * Wait-free → гарантирован прогресс каждого потока

2. **Сложность реализации**

   * Lock-free → проще
   * Wait-free → сильно сложнее, часто только для специальных структур

3. **Скорость**

   * Lock-free обычно быстрее
   * Wait-free может быть медленнее из-за overhead на гарантии

4. **Starvation**

   * Lock-free → поток может «зависнуть» в цикле
   * Wait-free → starvation невозможен

🔹 Примеры

Lock-free

```cpp
std::atomic<int> x{0};
int expected = 0;
while (!x.compare_exchange_weak(expected, expected + 1)) {}
```

* Прогресс гарантирован для системы
* Один поток может крутиться в цикле долго → starvation возможен

Wait-free

* Операции, где каждый поток завершает свою работу за фиксированное число шагов, например:

  * bounded ring buffer с atomic FAAArray
  * небольшие FAA counters

---

<a name="cache_coherence"></a>
### 🟢 Cache coherence (когерентность кеша)

🔹 Кратко

**Cache coherence** — это свойство мультипроцессорной системы, при котором **все ядра видят согласованное значение данных в памяти**, даже если каждый использует свой кеш.

*Cache coherence — это механизм, который гарантирует, что все ядра видят согласованные данные в памяти. Любые изменения одной линии кеша распространяются на другие ядра через протоколы MESI/MOESI. Атомарные операции используют это для корректного RMW без гонок.*

> Другими словами: когда один поток пишет в память, другие ядра видят это корректно, а не «старое» или «частично обновлённое» значение.

🔹 Зачем нужна

* Каждый CPU имеет **локальный кеш**
* Поток A меняет значение x
* Поток B может читать **старое значение**, если нет когерентности
* MESI / MOESI протоколы решают эту проблему

🔹 MESI протокол (типичный пример)

| Состояние     | Значение                               |
| ------------- | -------------------------------------- |
| **M**odified  | линия кеша изменена, эксклюзивная      |
| **E**xclusive | линия кеша не изменена, эксклюзивная   |
| **S**hared    | линия кеша читается несколькими ядрами |
| **I**nvalid   | линия кеша устарела, нужно обновить    |

* Когда поток пишет → другие кеши получают **invalidate** или **update**
* CPU гарантирует atomicity и последовательность

🔹 Связь с атомиками

* `std::atomic` использует **кеш-когерентность**, чтобы гарантировать корректные RMW операции
* Cache line bouncing — это прямое следствие когерентности при частых атомарных операциях между ядрами

---

<a name="contention"></a>
### 🟢 Что такое contention и как измерить?

*Contention измеряют по времени ожидания mutex, количеству CAS-retries, futex wait событиям и cache misses.<br>
Лучший способ — perf: futex_wait, cache-misses и масштабируемость throughput при росте потоков.*

🔹 **Contention** — это время/ресурсы, потраченные потоками **на ожидание** доступа к разделяемому ресурсу (mutex, atomic, cache line).

Измеряют:

1. **Время ожидания**
2. **Количество конфликтов**
3. **Низкоуровневые события CPU**

В коде (быстро и просто)

🔹 Mutex wait time

```cpp
auto t0 = std::chrono::high_resolution_clock::now();
m.lock();
auto t1 = std::chrono::high_resolution_clock::now();

wait_time += (t1 - t0);
```

* Работает для:

  * mutex
  * spinlock
* Минус: искажает тайминги

🔹 CAS retry count (lock-free)

```cpp
size_t retries = 0;
while (!x.compare_exchange_weak(exp, val)) {
    ++retries;
}
```

* Много retries → высокий contention
* Классика для lock-free очередей

🔹 Измерить: Linux `perf` (Через ОС и CPU (лучший способ))

```bash
perf stat -e \
  mutex_contention,\
  cache-misses,\
  LLC-load-misses \
  ./app
```

Ключевые события:

* `cache-misses`
* `LLC-load-misses`
* `cpu/mem-loads/`
* `sched:sched_switch`

🔹 perf record / report

```bash
perf record -g ./app
perf report
```

* Видно:

  * где спин
  * где блокировки
  * горячие mutex


🔹 futex wait (Специально для mutex / futex)

```bash
perf stat -e futex:futex_wait ./app
```

* Чем больше `futex_wait` → тем выше contention
* Отлично работает для `std::mutex`

---

## 5️⃣ Cache contention / false sharing

```bash
perf stat -e \
  cache-references,\
  cache-misses,\
  LLC-store-misses \
  ./app
```

Признаки:

* высокий `LLC-store-misses`
* рост latency при росте потоков

---

<a name="futex"></a>
### 🟢 Что делает futex и почему он быстрый?

**futex (fast userspace mutex)** — это **механизм синхронизации, который работает в основном в userspace** и обращается в ядро **только при необходимости**.

> Благодаря этому он быстрее обычных pthread mutex, особенно при низкой конкуренции.

🔹Как работает futex

1. **Userspace** (fast path)

   * Поток пытается захватить lock **без syscalls**
   * Обычная проверка `atomic` переменной:

     ```cpp
     if (lock == 0) { lock = 1; } // fast path
     ```
   * Если получилось → **нет перехода в ядро** → быстро

2. **Kernel space** (slow path)

   * Если lock занят → поток вызывает syscall `futex()`
   * Ядро ставит поток в **очередь ожидания**
   * Когда lock освобождается, ядро пробуждает один или несколько потоков

| Функция                    | Userspace | Kernel | Performance              |
| -------------------------- | --------- | ------ | ------------------------ |
| Захват lock без contention | ✅         | ❌      | Очень быстро             |
| Захват lock при contention | ❌         | ✅      | Зависит от scheduler     |
| Разбудить поток            | ❌         | ✅      | Только при необходимости |

---

<a name="userspace_blocking_vs_kernel_blocking "></a>
### 🟢 Чем userspace blocking отличается от kernel blocking

*Userspace blocking — это активное ожидание без перехода в ядро, с минимальной задержкой, но высокой загрузкой CPU.<br>
Kernel blocking усыпляет поток через syscall, отдавая CPU планировщику, что масштабируется лучше, но дороже по latency.<br>
Современные mutex используют гибридный подход через futex.*

* **Userspace blocking** — поток *логически* ждёт, но **не отдаёт CPU ядру**
* **Kernel blocking** — поток **усыпляется ядром**, CPU отдаётся другим

🔹Userspace blocking (busy wait / spin)

Поток:

* остаётся **RUNNABLE**
* крутится в цикле
* постоянно проверяет условие

```cpp
while (!flag.load(std::memory_order_acquire)) {
    // spin
}
```

Что происходит на CPU

* поток **всегда активен**
* выполняет инструкции
* держит cache lines
* участвует в cache-coherence

🔹Kernel blocking (sleep)

Поток:

* вызывает syscall (`futex`, `pthread_cond_wait`, `sleep`)
* ядро:

  * снимает поток с run queue
  * переводит в **SLEEPING / WAITING**

```cpp
std::unique_lock<std::mutex> lk(m);
cv.wait(lk);
```

Что происходит на CPU

* поток **не исполняется**
* CPU может выполнять другой код
* scheduler управляет fairness

Плюсы

* ✅ не жрёт CPU
* ✅ хорошо масштабируется
* ✅ подходит для долгого ожидания


**Userspace blocking vs Kernel blocking**

| Критерий                | Userspace blocking (spin / busy wait) | Kernel blocking (sleep / wait)   |
| ----------------------- | ------------------------------------- | -------------------------------- |
| Где происходит ожидание | Userspace                             | Kernel space                     |
| Состояние потока        | RUNNABLE                              | SLEEPING / WAITING               |
| Использование CPU       | 🔥 Высокое (крутится)                 | 💤 Нулевое                       |
| Syscall                 | ❌ Нет                                 | ✅ Есть                           |
| Context switch          | ❌ Нет                                 | ✅ Есть                           |
| Latency                 | ⚡ Минимальная                         | 🐢 Выше                          |
| Масштабируемость        | ❌ Плохая                              | ✅ Хорошая                        |
| Fairness                | ❌ Нет                                 | ⚠️ Частичная (scheduler)         |
| Cache line bouncing     | ❌ Часто                               | ✅ Нет                            |
| Энергопотребление       | ❌ Высокое                             | ✅ Низкое                         |
| Типичные примеры        | spinlock, busy loop                   | mutex, condition_variable, sleep |
| Подходит для            | Короткого ожидания                    | Долгого ожидания                 |
| Риск livelock           | ✅ Да                                  | ❌ Нет                            |