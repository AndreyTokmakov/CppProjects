Нет, **сам по себе `std::chrono::system_clock::now()` не вызывает context switch**.

Но здесь есть важный нюанс: `now()` обычно обращается к механизму получения времени ОС, и конкретная реализация может использовать системный вызов или VDSO.

### Linux

На современном Linux типичный путь выглядит примерно так:

```cpp
auto t = std::chrono::system_clock::now();
```

↓

```text
std::chrono::system_clock::now()
        ↓
clock_gettime(CLOCK_REALTIME, ...)
        ↓
VDSO
        ↓
CPU instructions
```

`clock_gettime()` **не обязательно делает syscall**.

Linux обычно предоставляет `clock_gettime()` через **vDSO (Virtual Dynamic Shared Object)** — кусок кода, отображённый непосредственно в address space процесса.

Поэтому:

```text
userspace
   │
   ├── system_clock::now()
   │
   ├── clock_gettime()
   │
   └── vDSO
         │
         └── чтение clock data + CPU instructions
```

И **context switch не происходит**.

### А если всё-таки будет syscall?

Даже syscall сам по себе **не означает context switch**.

Например:

```cpp
syscall(SYS_clock_gettime, ...);
```

происходит переход:

```text
User mode
    ↓
Kernel mode
    ↓
User mode
```

Это **mode switch**, а не context switch.

Context switch — это когда scheduler переключает выполнение с одного thread/process на другой:

```text
Thread A
   ↓
scheduler
   ↓
Thread B
```

Это существенно более дорогая операция.

### Для performance tuning

Для `system_clock::now()` полезно различать:

| Операция                          | Context switch |
| --------------------------------- | -------------: |
| `system_clock::now()` через vDSO  |              ❌ |
| `clock_gettime()` через vDSO      |              ❌ |
| настоящий `clock_gettime` syscall |             ❌* |
| syscall, который блокирует thread |       возможно |
| scheduler preemption              |              ✅ |
| `std::this_thread::sleep_for()`   |       обычно ✅ |
| blocking `read()` / `recv()`      |     возможно ✅ |

* Сам syscall — это **user/kernel mode transition**, но не context switch.
