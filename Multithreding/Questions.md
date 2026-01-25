# 🧵 Потенциальные вопросы на собеседовании (C++ Multithreading)

---

## 🟢 База / разогрев
1. [Что такое **thread-safety**?](#what_is_thread_safety)
2. Чем **concurrency** отличается от **parallelism**?
3. Что такое **critical section**?
4. Когда `std::thread` дешевле, чем `std::async`, и наоборот?
5. Что происходит при исключении внутри `std::thread`?
6. Можно ли копировать `std::thread`? Почему?
7. В чём разница между `detach()` и `join()`?
8. Почему `sleep_for` — плохая синхронизация?
9. Можно ли безопасно завершить detached thread?
10. Что произойдёт, если два потока пишут в `std::vector`?

---

## 🟡 Mutex / Locks
11. Чем `std::mutex` отличается от `std::recursive_mutex`?
12. Когда `recursive_mutex` — плохая идея?
13. Что такое **lock convoy**?
14. Зачем нужен `std::try_lock`?
15. Можно ли захватить mutex в signal handler?
16. Можно ли использовать один mutex для разных данных?
17. Почему long-held mutex — антипаттерн?
18. Чем `std::scoped_lock` лучше `std::lock_guard`?
19. Что такое priority inversion?
20. Как priority inversion решается?

---

## 🟠 Condition Variables / Coordination
21. Почему `condition_variable` не хранит состояние?
22. Что произойдёт, если вызвать `notify_one` без mutex?
23. Почему `notify_all` может быть опасен?
24. Что будет, если `notify` вызван до `wait`?
25. Можно ли использовать `condition_variable` без mutex?
26. Чем `condition_variable_any` хуже?
27. Когда лучше использовать `latch` или `barrier`?
28. Можно ли реализовать semaphore через `condition_variable`?

---

## 🟠 Atomics / Memory Model
29. В чём разница между **atomicity** и **visibility**?
30. Почему `std::atomic<T>` не всегда lock-free?
31. Что значит `is_lock_free()`?
32. Когда `compare_exchange_weak` предпочтительнее `strong`?
33. Может ли `atomic<int>` привести к cache line bouncing?
34. Почему atomic может быть медленнее mutex?
35. В чём разница между acquire/release и seq_cst?
36. Можно ли смешивать разные memory_order?
37. Что такое **out-of-thin-air reads**?
38. Почему UB из-за data race хуже segfault?
39. Почему CAS может спуриться (fail spuriously)?
40. Что такое release sequence?

---
## 🔴 Продвинутые темы
41. Что такое **ABA problem** и где она возникает?
42. Почему lock-free ≠ fast?
43. Что такое **wait-free progress guarantee**?
44. Что сложнее: MPMC queue или SPSC queue?
45. Почему memory reclamation — самая сложная часть lock-free?
46. Что такое hazard pointers?
47. Чем RCU отличается от mutex?
48. Что такое quiescent state?
49. Почему lock-free код сложнее тестировать?
50. Можно ли сделать lock-free `std::vector`?

---

## 🔥 Производительность / Архитектура
51. Что такое cache coherence?
52. MESI протокол — кратко
53. Почему false sharing опаснее, чем contention?
54. Что такое NUMA?
55. Как NUMA влияет на mutex?
56. Когда pinning потоков полезен?
57. Почему `malloc/free` плохо масштабируются?
58. Как аллокаторы влияют на multithreading?
59. Почему shared_ptr дорогой в MT?
60. Как измерить contention?

---

## 🧪 Debugging / Production
61. Как найти data race в проде?
62. Почему race может исчезнуть при логировании?
63. Как ThreadSanitizer влияет на memory ordering?
64. Можно ли полагаться на тесты для MT кода?
65. Почему MT баги не воспроизводятся?
66. Как писать unit-тесты для concurrent кода?
67. Как безопасно останавливать сервис с потоками?
68. Что происходит при `fork()` в многопоточном процессе?
69. Что такое async-signal-safe?
70. Можно ли ловить deadlock автоматически?

---

## 🧠 Вопросы «на мышление»
71. Как бы ты объяснил memory model джуниору?
72. Что ты выберешь: проще код или быстрее?
73. Когда лучше отказаться от lock-free?
74. Какие MT баги ты ловил лично?
75. Как бы ты спроектировал очередь для HFT?
76. Как защитить read-mostly структуру?
77. Как масштабировать логгер на 100+ потоков?
78. Что опаснее: race или deadlock — и почему?
79. Какой самый сложный concurrent баг ты видел?
80. Какие MT темы ты считаешь «красным флагом»?

---

<a name="what_is_thread_safety"></a>
### Что такое thread-safety?

Thread-safety — это свойство кода или объекта корректно работать при одновременном доступе из нескольких потоков,
без data race и UB, сохраняя свои инварианты.


Когда std::thread дешевле, чем std::async, и наоборот?

std::thread дешевле и предсказуемее,
std::async удобнее, но может быть дороже и менее контролируем.

🟢 Когда std::thread дешевле
1️⃣ Нужен полный контроль над потоком
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


Можно ли копировать std::thread? Почему?


	Нет, std::thread нельзя копировать.
	Его можно только перемещать.


🟡 Mutex / Locks

Чем std::mutex отличается от std::recursive_mutex?
Когда recursive_mutex — плохая идея?
Что такое lock convoy?
Зачем нужен std::try_lock?
Можно ли захватить mutex в signal handler?
Можно ли использовать один mutex для разных данных?
Почему long-held mutex — антипаттерн?
Чем std::scoped_lock лучше std::lock_guard?
Что такое priority inversion?
Как priority inversion решается?

Что такое lock convoy?

	Lock convoy — это ситуация, когда несколько потоков одновременно пытаются захватить один mutex,
	и каждый поток вынужден ждать, что вызывает длительные задержки и деградацию производительности.

	Проще: очередь потоков за mutex, где каждый поток блокируется и пропускает CPU, создавая “конвой” потоков.

	Lock convoy — это цепочка потоков, блокирующихся на одном mutex, когда каждый ждёт своего хода, что вызывает задержки и падение производительности. Проблему решают уменьшением критических секций, использованием нескольких mutex или lock-free структур.



🟠 Condition Variables / Coordination

Почему condition_variable не хранит состояние?

Что произойдёт, если вызвать notify_one без mutex?

Почему notify_all может быть опасен?

Что будет, если notify вызван до wait?

Можно ли использовать condition_variable без mutex?

Чем condition_variable_any хуже?

Когда лучше использовать latch или barrier?

Можно ли реализовать semaphore через condition_variable?

🟠 Atomics / Memory Model

В чём разница между atomicity и visibility?

Почему std::atomic<T> не всегда lock-free?

Что значит is_lock_free()?

Когда compare_exchange_weak предпочтительнее strong?

Может ли atomic<int> привести к cache line bouncing?

Почему atomic может быть медленнее mutex?

В чём разница между acquire/release и seq_cst?

Можно ли смешивать разные memory_order?

Что такое out-of-thin-air reads?

Почему UB из-за data race хуже segfault?

Почему CAS может спуриться (fail spuriously)?

Что такое release sequence?

🔴 Продвинутые темы

Что такое ABA problem и где она возникает?

Почему lock-free ≠ fast?

Что такое wait-free progress guarantee?

Что сложнее: MPMC queue или SPSC queue?

Почему memory reclamation — самая сложная часть lock-free?

Что такое hazard pointers?

Чем RCU отличается от mutex?

Что такое quiescent state?

Почему lock-free код сложнее тестировать?

Можно ли сделать lock-free std::vector?

🔥 Производительность / Архитектура

Что такое cache coherence?

MESI протокол — кратко

Почему false sharing опаснее, чем contention?

Что такое NUMA?

Как NUMA влияет на mutex?

Когда pinning потоков полезен?

Почему malloc/free плохо масштабируются?

Как аллокаторы влияют на multithreading?

Почему shared_ptr дорогой в MT?

Как измерить contention?

🧪 Debugging / Production

Как найти data race в проде?

Почему race может исчезнуть при логировании?

Как ThreadSanitizer влияет на memory ordering?

Можно ли полагаться на тесты для MT кода?

Почему MT баги не воспроизводятся?

Как писать unit-тесты для concurrent кода?

Как безопасно останавливать сервис с потоками?

Что происходит при fork() в многопоточном процессе?

Что такое async-signal-safe?

Можно ли ловить deadlock автоматически?

🧠 Вопросы «на мышление»

Как бы ты объяснил memory model джуниору?

Что ты выберешь: проще код или быстрее?

Когда лучше отказаться от lock-free?

Какие MT баги ты ловил лично?

Как бы ты спроектировал очередь для HFT?

Как защитить read-mostly структуру?

Как масштабировать логгер на 100+ потоков?

Что опаснее: race или deadlock — и почему?

Какой самый сложный concurrent баг ты видел?

Какие MT темы ты считаешь «красным флагом»?