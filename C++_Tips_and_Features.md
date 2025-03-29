### [C++ 17](https://en.cppreference.com/w/cpp/17)
_Libraries:_
- std::any
- std::string_view
- std::variant
- [std::pmr::memory_resource](https://en.cppreference.com/w/cpp/memory/memory_resource)
- std::optional
- std::filesystem

_Features:_
- Structured binding declaration 
- compile-time if constexpr
- Fold-expressions
- Stricter expression evaluation order
- Class template argument deduction (CTAD) 
- Dynamic Memory Allocation for Over-Aligned Data
- initializers for if and switch
- guaranteed copy elision
- Nested Namespaces
- if constexpr
- inline variables
- new attributes: `[[fallthrough]]` `[[maybe_unused]]` `[[nodiscard]]`
- Placement new

### [C++ 20](https://en.cppreference.com/w/cpp/20)
_Libraries:_
- [concepts](https://en.cppreference.com/w/cpp/concepts)
- [std::format](https://en.cppreference.com/w/cpp/utility/format/format)
- [std::source_location](https://en.cppreference.com/w/cpp/utility/source_location)
- [std::span](https://en.cppreference.com/w/cpp/header/span)
- [modules](https://en.cppreference.com/w/cpp/language/modules)
- [ranges](https://en.cppreference.com/w/cpp/ranges)
- [coroutines](https://en.cppreference.com/w/cpp/language/coroutines)

_Features:_
- 3-way comparison
- New attributes: `[[no_unique_address]]`, `[[likely]]`, `[[unlikely]]`
- std::ssize(v) --> signed size
- Constexpr Improvements [constexpr Dynamic Memory Allocation, C++20]
- Template Syntax For Generic Lambdas
- Constrained Auto                           //  void foo(std::floating_point auto fp) { ... } 
- Template Syntax For Generic Lambdas        //  auto fn = []<typename T>(vector<T> const& vec) { .... }
- Range-based for loop with Initializer      //  for (init; decl : expr) { .... }
- Designated Initializers                    //  Type obj = { .designator = val, .designator { val2 }, ... };
- consteval, constinit
- 'using enum':  using enum long_enum_name;  //  control the visibility of enumerator names and thus make it simpler to write.
- NTTP : Class-types in non-type template parameters 
- Prefixes and suffixes for std::strings     //  starts_with() and .ends_with()
- Containers, checking for elements          //  contains() member function <br>

_Multithreading:_
- [std::stop_token](https://en.cppreference.com/w/cpp/thread/stop_token)
- [std::semaphore](https://en.cppreference.com/w/cpp/thread/counting_semaphore)
- [std::latch](https://en.cppreference.com/w/cpp/thread/latch)
- [std::barrier](https://en.cppreference.com/w/cpp/thread/barrier)
- [std::jthread ](https://en.cppreference.com/w/cpp/thread/jthread)
 
Math:
- midpoint and lerp
- Math Constants [pi_v ...]

### [C++ 23](https://en.cppreference.com/w/cpp/23)
- [really](https://www.kdab.com/cpp23-will-be-really-awesome)  (explicit const)
- static_assert (false) in templates           //  now compiles!!!
- std::flat_map
- std::flat_set
- [std::expected](https://en.cppreference.com/w/cpp/utility/expected)
- [std::stacktrace](https://en.cppreference.com/w/cpp/header/stacktrace)
- std::spanstream
- std::print
- Relaxing constexpr restrictions

	[ allows using static constexpr variables in constexpr functions ]

• Static operators
`																	   void some_test_func() 
    struct S {                                                             constexpr S s;
        static constexpr bool operator() (int x, int y) {              	   static_assert (s (1, 2)); 
            return x < y;                                                  
        }                                                                  constexpr bool result = S::operator()(1, 2);  // OK in C++23	 
    };																	   std::cout << std::boolalpha << result << std::endl; 		
                                                                       }`
• 'Extend init-statement to allow alias-declaration'
	
`for (using T = int; T e : v) {}`

• 'Attributes on Lambda-Expressions' [https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2173r1.pdf] 


• std::start_lifetime_as | std::start_lifetime_as_array

	'std::start_lifetime_as' solves the cast-bytes-as-POD use case, and it makes sense. 
	But there is so much 'reinterpret_cast' and C-style cast code in existence to perform the same task, and they don’t just suddenly become valid with '-std=c++23'. 
	Effort must be spent to migrate those casts to 'std::start_lifetime_as' case by case: not every 'reinterpret_cast' is meant to start lifetime.
	Also 'start_lifetime_as' needs to be taught (even it’s easy to learn). The overall cost to the user base is not small.

### [C++ 26](https://en.cppreference.com/w/cpp/compiler_support/26)
• Trivial relocatability for C++

• std::hive (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0447r28.html#introduction)

• Feature testing  (since C++20) --> https://en.cppreference.com/w/cpp/feature_test#cpp_lib_freestanding_functional

• [RCU](https://en.cppreference.com/w/cpp/header/rcu)

• 'Specifying a reason for deleting a function'

   struct NonCopyable {
	  NonCopyable(const NonCopyable&) = delete("Since this class manages unique resources, copy is not supported; use move instead.");
   }

• 'Structured binding declaration as a condition' | IF statement structured binding initialization

	if(auto [position, length] = get_next_token(text, offset); position >= 0)      | 	for (auto [position, length] : tokenize(text, offset)) 
	{                                                                              |	{
  		std::println("pos {}, len {}", position, length);                          |		std::println("pos {}, len {}", position, length);
	}                                                                              |  	}

• 'user-generated static_assert messages'

	static_assert(sizeof(int) == 4, std::format("Expected 4, actual {}", sizeof(int)));

• 'Variadic friends'
                                                             template<class... Ts>
	template<class T>                               |		 struct VS {
	struct C {                                      |			 template<class U>
  		template<class U> struct Nested;            |			 friend class C<Ts>::Nested...; // OK
	};                                              |        };

============================================================================================================================================================	
				                           			           	      C++ Idioms  
============================================================================================================================================================

• RAII   (Resource Acquisition Is Initialization)
• CRTP   (Curiously Recurring Template Pattern)
• Pimpl  (Pointer to Implementation)
• NVI    (Non-Virtual Interface)
• SFINAE (Substitution Failure Is Not An Error)
• Copy-and-swap
• Type Erasure

============================================================================================================================================================	
				                           			           	      Hashing 
============================================================================================================================================================

	A hash function is any function that can be used to map data of arbitrary size to fixed-size values
	The values returned by a hash function are called hash values, hash codes, digests, or simply hashes
	
	Хеш-функции применяются в следующих случаях:
		• при построении ассоциативных массивов;
		• при поиске дубликатов в сериях наборов данных;
		• при построении уникальных идентификаторов для наборов данных;
		• при вычислении контрольных сумм от данных (сигнала) для последующего обнаружения в них ошибок 
		  (возникших случайно или внесённых намеренно), возникающих при хранении и/или передаче данных;
		• при сохранении паролей в системах защиты в виде хеш-кода (для восстановления пароля по хеш-коду требуется функция,
		   являющаяся обратной по отношению к использованной хеш-функции);
		• при выработке электронной подписи (на практике часто подписывается не само сообщение, а его «хеш-образ»);

	Существует множество алгоритмов хеширования, отличающихся различными свойствами:
		• разрядность;
		• вычислительная сложность;
		• криптостойкость.
		
		
	Хеш-табли́ца — это структура данных, реализующая интерфейс ассоциативного массива, а именно, 
	              она позволяет хранить пары (ключ, значение) и выполнять три операции: операцию добавления новой пары, операцию поиска и операцию удаления пары по ключу.

	Сложность в О-символике:	В среднем	В худшем случае
				Расход памяти	  O(n)	     O(n)
				Поиск			  O(1)	     O(n)
				Вставка			  O(1)	     O(n)
				Удаление	      O(1)	     O(n)
============================================================================================================================================================	
				                    Memory Layout of a C program | .text  | /home/andtokm/Documents/Books/Memory_layout_on_x86_64.pdf
============================================================================================================================================================
			
A typical memory map of a C program consists of the following sections:

					• A text segment
					• Initialized data segment
					• Uninitialized data segment
					• Stack
					• Heap

• (.text) | Text Segment

	Также называется сегментом кода (Code), используется для хранения кода выполнения программы и может также содержать некоторые константы (например, строковые константы и т. Д.). 
	Эта память выделяется статически и предназначена только для чтения (некоторые архитектуры могут допускать модификацию)
	Эта память является общей - когда существует несколько процессов, они используют один и тот же текстовый сегмент.

	Все функции это по факту блок кода в .text section, то есть просто в области, где находится код. SH

• (.data) | Initialized Data segment 

	A data segment (.data) contains the global variables and static variables that are 'initialized' and starts right after '.text'
	To see an example let’s add a global variable into our code:
	Contains initialized global variables and static variables.

	Can be further categorized into 2 parts :
		Initialized read-only area: e.g.  // const float PI = 3.142;
		Initialized read-write area : 	  // int num = 50;	

• (.bss) | 	BSS | Uninitialized Data Segment

	'Uninitialized' data segment or bss segment, named after an ancient assembler operator that stood for 'Block Started by Symbol'
	Contains global and static variables that are not explicitly initialized in the source code
	This segment starts at the end of the '.data' segment and contains all global and static variables that 'do not have explicit initialization'. bss also end up in RAM. 

• Stack

	Contains local function variables and other function related data
	
	-	stack segment grows downwards. Of course, there is a reason behind this design, and the growing direction is architecture-dependent too. 
	-	everything pushed onto the stack segment results in a lower address in most architectures. 
	-	Thus, the “top of the stack” actually located at lower address in the memory.

	- Stack segment is a block of memory used as a temporary storage to store theprogram stack (a.k.a call stack) of an executed program. 
	- Program stack is just a collection of stack frames holding function related data including function parameters, return address, and local variables.

• Heap

	Contains dynamic allocated data (data allocated at runtime).


============================================================================================================================================================	
				                          Calling Convention | Function call | Как происходит вызов функции
============================================================================================================================================================		

Аппаратный стек — это непрерывная область памяти, аппаратно поддерживаемая центральным процессором и адресуемая специальными регистрами:
	ss (сегментный регистр стека),
	bp (регистр указателя базы стекового кадра), [ebp] / [rbp]
	sp (регистр указателя стека), последний хранит адрес вершины стека (смещение относительно сегмента стека). [esp] / [rsp]

Чтобы выделить память на стеке достаточно просто сместить указатель вершины стека на требуемый размер, чтобы освободить память, нужно вернуть указатель в исходное положение.

									foo():
void foo()  {                    |        push    rbp
    int a = 1; 		             |        mov     rbp, rsp
    int b = 2;      	         |        mov     dword ptr [rbp - 4], 1
    bar(a, b);                   |        mov     dword ptr [rbp - 8], 2
}		            		     |        pop     rbp
            		             |        ret
int main() {                     |
    foo();                       |	main:
}                                |        push    rbp
								 |        call    foo()
								 |        xor     eax, eax
								 |        pop     rbp
								 |        ret

1. Начало работы функции main, на стеке лежит адрес возврата из функции
2. пушим на стек значение регистра ebp (указатель базы стекового кадра) т.к. дальше значение регистра будет меняться на базу текущего стекфрейма
3. сохраняем в ebp значение регистра esp (адрес вершины стека)

- Все параметры требуемые для вызова функции push-атся на стек в 'обратом порядке' относительно того как они определены в сигратуре функции
- После всех параметров на стек таже push-атся адрес возврата


							     |	square:             ; x = edi, ret = eax		
int square(int x) 			     |		imul edi, edi	     
{		    					 |		mov  eax, edi
    return x * x;		         |		ret
}		  					     |



============================================================================================================================================================	
				                           			           	      C/C++ Struct Padding
============================================================================================================================================================		
	
// https://hitsoda.medium.com/c-c-struct-padding-fab7c6ae4e11
// https://www.linkedin.com/pulse/why-structure-padding-required-c-arsalan-saleem/

Padding is a concept that the compiler inserts some extra spaces between members of a struct for alignment.
The alignment rules:
	• Size of the whole struct is divisible by size of the largest individual member in the struct. 
	  If the largest member’s size is 4, the size of the struct should be 4xN such as 0, 4, 8, 12, 16, … .

	• Each individual member must be located at an address that is divisible by its size.
	  A long  member must be located at an address divisible by 8. 
	  A int   member must start at an address divisible by 4. 
	  A short member must start at an even address. 
	  A char  member can start at any address.


struct mystruct {                     0       1       2       3       4       5       6       7       8
    char a;                           +-------+-------+-------+-------+-------+-------+-------+-------+
    int b;                            |a=0xff |    padding=0x000000   |           b=0xffffffff        |
};                                    +-------+-------+-------+-------+-------+-------+-------+-------+


CPU когда читат к примеру 'char a' и 'int b' в случае с Padding-ом то используется одно обращение к памяти для каждой переменной
если БЕЗ Padding-а нужно прочесть 'char a' и выполнять byte shift (чтобы получить чась для char-а) и так же часть 'int b' находится 
в области с блока 'char a' и частично в 'int b'.

============================================================================================================================================================	
				                           			           	      Multi-threading
============================================================================================================================================================		
		
• Существует два типа потоков: UserSpace и Kernel потоки

• Critical section:
	Объект синхронизации потоков, позволяющий предотвратить одновременное выполнение некоторого набора операций (обычно связанных с доступом к данным) несколькими потоками
	Критическая секция выполняет те же задачи, что и мьютекс.
					
• Mutex:
	Класс mutex является примитивом синхронизации, который может использоваться для защиты разделяемых данных от одновременного доступа нескольких потоков.
	mutex предлагает эксклюзивую, нерекурсивную семантику владения:
	- Вызывающий поток владеет мьютексом со времени успешного вызова lock или try_lock, и до момента вызова unlock.
	- Пока поток владеет мьютексом, все остальные потоки при попытке завладения им блокируются на вызове lock или получают значение false при вызове try_lock.
	- Вызывающий поток не должен владеть мьютексом до вызова lock или try_lock.
		

• std::lock_guard 

	The class lock_guard is a mutex wrapper that provides a convenient RAII-style mechanism for owning a mutex for the duration of a scoped block.
	When a lock_guard object is created --> locks the mutex,  When control leaves the scope --> the mutex is released.
	The lock_guard class is non-copyable.

• std::unique_lock

	The class unique_lock is movable, but not copyable
	shall be used with 'std::condition_variable'

• std::scoped_lock

	mutex wrapper that provides a convenient RAII-style mechanism for owning ZERO or MORE mutexes for the duration of a scoped block.
	The scoped_lock class is non-copyable.
		
• std::latch

	single-use thread barrier
	The latch class is a downward counter which can be used to synchronize threads.
	The value of the counter is initialized on creation.
	Threads may block on the latch until the counter is decremented to zero.
	There is no possibility to increase or reset the counter, which makes the latch a single-use barrier.

• std::barrier
	reusable thread barrier
	The class template std::barrier provides a thread-coordination mechanism that blocks a group of threads of known size until all threads in that group have reached the barrier
	Unlike std::latch, barriers are reusable: once a group of arriving threads are unblocked, the barrier can be reused. 
	Unlike std::latch, barriers execute a possibly empty callable before unblocking threads.
  

• What wrong with this code: [лок не сработает - как как переменная std::unique_lock<std::mutex> не именована]
	void Foo() noexcept {
		std::unique_lock<std::mutex>(m_mutex);
		do_somethig();
	}
		
• What is Semaphore:
	Semaphore is simply a variable that is non-negative and shared between threads. 
	A semaphore is a signaling mechanism, and a thread that is waiting on a semaphore can be signaled by another thread. 
	It uses two atomic operations, 1) wait
									   2) signal for the process synchronization.
	A semaphore either allows or disallows access to the resource, which depends on how it is set up.
		
	Here, are characteristic of a semaphore:
	- It is a mechanism that can be used to provide synchronization of tasks.
	- It is a low-level synchronization mechanism.
	- Semaphore will always hold a non-negative integer value.
	- Semaphore can be implemented using test operations and interrupts, which should be executed using file descriptors.
		
	Types of Semaphores
	- Counting semaphores
	- Binary semaphores.


▪ Модель памяти: [ 'https://en.cppreference.com/w/cpp/atomic/memory_order' ]

	All modifications of a single atomic object are totally ordered

	• This is called the modification order of the object
	• All threads are guaranteed to observe modifications of the object in this order

	Modifications of different atomic objects may be unordered

	• Different threads may observe modifications of multiple atomic objects in a different order
	• The details depend on the memory order that is used for the atomic operations

	relaxed | memory_order_relaxed:

		Relaxed operation: there are no synchronization or ordering constraints imposed on other reads or writes, 
		only this operation-s atomicity is guaranteed: https://en.cppreference.com/w/cpp/atomic/memory_order#Relaxed_ordering

		гарантирует только свойство атомарности операций, при этом не может участвовать в процессе синхронизации данных между потоками
		- модификация переменной "появится" в другом потоке не сразу
		- поток thread2 "увидит" значения одной и той же переменной в том же порядке, в котором происходили её  модификации в потоке thread1
		- порядок модификаций разных переменных в потоке thread1 не сохранится в потоке thread2

	release/acquire:
		Флаг синхронизации памяти acquire/release является более тонким способом синхронизировать данные между парой потоков. 
		Два ключевых слова: memory_order_acquire и memory_order_release работают только в паре над одним атомарным объектом.
		- модификация атомарной переменной с release будет видна видна в другом потоке, выполняющем чтение этой же атомарной переменной с acquire
		- все модификации памяти в потоке thread1, выполняющей запись атомарной переменной с release, будут видны после выполнения чтения той же переменной с acquire в потоке thread2
		- процессор и компилятор не могут перенести операции записи в память раньше release операции в потоке thread1, и нельзя перемещать выше операции чтения из памяти позже acquire операции в потоке thread2

	sequential consistency: [std::memory_order_seq_cst]
		"Guarantees that all threads see all atomic operations in one globally consistent order"
		Флаг синхронизации памяти "единая последовательность" (sequential consistency, seq_cst) дает самые строгие:
		- порядок модификаций разных атомарных переменных в потоке thread1 сохранится в потоке thread2
		- все потоки будут видеть один и тот же порядок модификации всех атомарных переменных. Сами модификации могут происходить в разных потоках
		- все модификации памяти (не только модификации над атомиками) в потоке thread1, выполняющей store на атомарной переменной, 
		  будут видны после выполнения load этой же переменной в потоке thread2

	    Этот флаг синхронизации памяти в C++ используется по умолчанию, т.к. с ним меньше всего проблем с точки зрения корректности выполнения программы. 
	    Но seq_cst является дорогой операцией для процессоров, в которых вычислительные ядра слабо связаны между собой 
	    в плане механизмов обеспечения консистентности памяти. Например, для x86-64 seq_cst дешевле, чем для ARM архитектур.


• Cache lines:

	The data in a cache is grouped into blocks called cache-lines, which are typically 64 or 128 bytes wide. 
	These are the smallest units of memory that can be read from, or written to, main memory. 
	This works well in most programs as data that is close in memory is often needed close in time by a particular thread. 
	However, this is the root of the false sharing problem.


• Cache coherence:

	When a program writes a value to memory, it goes firstly to the cache of the core that ran the code. 
	If any other caches hold a copy of that cache line, their copy is marked as invalid and cannot be used. 
	The new value is written to main memory, and the other caches must re-read it from there if they need it. 
	Although this synchronization is implemented in hardware, it still takes time. 
	And, of course, reading from main memory takes a few hundred clock cycles by itself.

	Modern processors use the MESI protocol to implement cache coherence. This basically means each cache line can be in one of four states:

		- Modified
		- Exclusive
		- Shared
		- Invalid

	When a core modifies any data in a cache line, it transitions to "Modified", and any other caches that hold a copy of the same cache line are forced to "Invalid". 
	The other cores must then read the data from main memory next time they need it. 


▪ False Sharing:

		False sharing is also known as cache-line ping-ponging. 
		False sharing is one of the well-known performance issues on multi-core systems, where each cpu has its local cache.

		False sharing occurs when threads on different processor modify variables that reside on same 'cache line'

		False Sharing refers to a scenario where different threads invalidate and update the same cache line, even though they are only modifying separate variables. 


		It is caused by one or more cores repeatedly invalidating the caches of the other cores, even while accessing isolated state.
		False sharing is very hard to detect because the threads may be accessing completely different global variables that happen to be relatively close together in memory.
		This forces the other cores to read data from main memory instead of their local cache, which slows them down considerably

		- True sharing  (истинное разделение) – это когда потоки имеют доступ к одному и тому же объекту памяти, например, общей переменной или примитиву синхронизации. 
		- False sharing (от лукавого)         – это доступ к разным данным, но по каким-то причинам, оказавшимся в одной кэш-линии процессора. 

		и тот, и другой случай вредит производительности из-за необходимости аппаратной синхронизации кэш-памяти процессора, 
		однако если первый случай часто неизбежен, то второй можно и нужно исключать.

		# https://medium.com/@joao_vaz/caches-and-the-problem-of-false-sharing-a-primer-in-c-17-1177ad07a625

		---- example of false sharing: 
		// In this example, two threads are updating two different instances of the Foo struct.
		// The struct has two integer fields x and y, which are accessed in a loop.
		// The loop alternates between updating x and y, which leads to false sharing because x and y are located on the same cache line.

	
		In computer science, false sharing is a performance-degrading usage pattern that can arise in systems with distributed, coherent caches at the size 
		of the smallest resource block managed by the caching mechanism. 
		When a system participant attempts to periodically access data that is not being altered by another party, but that data shares a cache block with
		data that is being altered, the caching protocol may force the first participant to reload the whole cache block despite a lack of logical necessity.
		The caching system is unaware of activity within this block and forces the first participant to bear the caching system overhead required by true shared access of a resource.


• Как работает spinlock:

		Spinlock - это примитив синхронизации, который используется для организации доступа к общему ресурсу между несколькими потоками в многопоточной среде.
		Он работает по принципу активного ожидания (busy-waiting), когда поток, не получив доступ к ресурсу, повторно проверяет условие блокировки без перехода в режим ожидания или сна.

		Основная идея spinlock состоит в том, чтобы поток, желающий получить доступ к ресурсу, повторял быструю проверку состояния блокировки в цикле, называемом "spin".
		Если блокировка уже удерживается другим потоком, текущий поток продолжает повторять проверку до тех пор, пока блокировка не будет освобождена.

		Однако, важно отметить, что использование spinlock не всегда является эффективным.
		Если потоки занимаются активным ожиданием без перехода в режим ожидания, это может привести к высокому использованию процессорного времени.
		Spinlock особенно полезен, когда блокировки очень короткие и вероятность конфликтов невысока.


• compare_exchange_strong VS compare_exchange_weak: [std::atomic]

		Обе функции выполняются атомарно
		Семантика работы операции compare_exchange_weak похожа на compare_exchange_strong.

		Разница лишь в том, что для compare_exchange_weak возможны ложные срабатывания.
		Они бывают вызваны тем, что на некоторых процессорных архитектурах используются инструкции LL/SC (Load-Linked/Store-Conditional) вместо CAS для решения так называемой ABA-проблемы.

		Из-за такой особенности compare_exchange_weak должен быть использован в цикле:

		expected = false;
		while (!current.compare_exchange_weak(expected, true)) {
			expected = false; // Restore 'expected' value to the one we expecting
		}

=============================================================================================================================================================
										Temporal locality | Spatial locality
=============================================================================================================================================================	

• temporal locality:

	means that if the processor is currently accessing a certain memory address
	there is a high probability it will access the same memory address in the near future (think a counter in a loop). 

• spatial locality:

	means is that if the processor is currently accessing a certain memory address
	there is a high probability it will access neighboring memory addresses in the near future (think running through arrays). 

=============================================================================================================================================================
																		Mutex | implementation							
=============================================================================================================================================================	

• Как реализованы std::mutext, std::atomic

	The two most common ways to implement mutexes in the hardware is to use either the (CPU features):
	1.  test-and-set (TAS)
	2.  compare-and-swap (CAS)


• TAS: test-and-set 

	In computer science, the test-and-set instruction is an instruction used to write (set) 1 to a memory location and return its old value as a single atomic operation.


• Is TAS a spinlock?

	The Test-And-Set (TAS) Lock is the simplest possible spinlock implementation.
	It uses a single shared memory location for synchronization, which indicates if the lock is taken or not. 
	The memory location is updated using a test-and-set ( TAS ) operation.Is test and set spinlock?

============================================================================================================================================================	
/**					                           			  Rule of Three   Rule of Five                                                                     **/
============================================================================================================================================================

	**** Prefer Rule of Zero when you can, use Rule of Five when you must. ****

	Each class should explicitly define exactly one of the following set of special member functions:

	• None
	• Destructor, copy constructor, copy assignment operator
	• In addition, each class that explicitly defines a destructor may explicitly define a move constructor and/or a move assignment operator.

	Usually, one of the following sets of special member functions is sensible:
	• None (for many simple classes where the implicitly generated special member functions are correct and fast)
	• Destructor, copy constructor, copy assignment operator (in this case the class will not be movable)
	• Destructor, move constructor, move assignment operator (in this case the class will not be copyable, useful for resource-managing classes where the underlying resource is not copyable)
	• Destructor, copy constructor, copy assignment operator, move constructor (because of copy elision, there is no overhead if the copy assignment operator takes its argument by value)
	• Destructor, copy constructor, copy assignment operator, move constructor, move assignment operator

    !!!! Если переопределяем один из методов - нужно переопределять все!
	     Иначе:
		       -  Если определим к примеру Dtor() ---> не будут определены move операции!

	class Object {
		/** Object copy constructor: **/
		Object(const Object&) = default;  
		/** Move constructor: **/
		Object(Object&&) = default;
		
		/** Object copy assignment operator: **/	
		Object& operator=(const Object&) = default;
		/** Move assignment operator: **/
		Object& operator=(Object&&) = default;
		
		/** Destructor: **/
		virtual ~Object() { } 
	};
	
	• Не заданный явно copy assignment operator будет установлен компилятором в delete в случаях если:
		- Класс имеет не статическую переменую of reference type
		- Класс имеет не статическую const переменую
		- Класс имеет явно заданный move constructor
		- Класс имеет явно заданный move assignment operator
		
	• Deleted implicitly-declared copy assignment operator
	  A implicitly-declared copy assignment operator for class T is defined as deleted if any of the following is true:
	  - T has a user-declared move constructor;
	  - T has a user-declared move assignment operator.  
	  A defaulted copy assignment operator for class T is defined as deleted if any of the following is true:
	  - T has a non-static data member of non-class type (or array thereof) that is const;
	  - T has a non-static data member of a reference type;
	  - T has a non-static data member or a direct or virtual base class that cannot be copy-assigned (overload resolution for the copy assignment fails, or selects a deleted or inaccessible function);
	  - T is a union-like class, and has a variant member whose corresponding assignment operator is non-trivial.


	▪ Что будет вызвано попытке std::move() объект класса у которого есть явно определённый destructor()

		Test {
	  		inline ~Test() = default;
	  		// inline constexpr Test(const Test &) noexcept = default;  <------------------ this will be generated()
		};

		int main()
		{
			Test t  = {};
			Test t2 = Test(static_cast<const Test&&>(std::move(t)));
		}

		// https://andreasfertig.blog/2022/03/a-destructor-default-and-the-move-operations/

		
============================================================================================================================================================	
    				                           			  Move senmantics                                                                     
============================================================================================================================================================
		
 ▪  What is it?
	While std::move() is technically a function - I would say it isn't really a function.
	Its sort of a converter between ways the compiler considers an expression's value.	
		
 ▪  move просто конвертируем LRef --> RRef
        void func(std::string&& str) {}
		std::string str;
		func(std::move(str)); // без move не заведётся

 ▪  What does it do?
	The first thing to note is that std::move() doesn't actually move anything. 
	It converts an expression from being an lvalue (such as a named variable) to being an xvalue. 
	An xvalue tells the compiler: You can plunder me, move anything Im holding and use it elsewhere (since I'm going to be destroyed soon anyway)
	
	in other words, when you use std::move(x), you are allowing the compiler to cannibalize x. 
	Thus if x has, say, its own buffer in memory - after std::move()ing the compiler can have another object own it instead.
	
 ▪  What is the Universal references:
	   T&& is either Rvalue reference or Lvalue reference: For example:
	
	   auto&& var2 = var1;                     // Universal references
	   template<typename T> void f(T&& param); // Universal references
	   
	   template<typename T> void f(std::vector<T>&& param); // RValue references
	   void f(Widget&& param);                              // RValue references
	   Widget&& var1 = Widget();                            // RValue references
	
 ▪  Can we move const object:
    NO. We can try to move it - it will complite but copy ctor will be called instead.
	
 ▪  Issues:
    - const disables move semantics
	
 ▪  std::forward VS move:

 	 - Функция std::forward, как известно, применяется при идеальной передаче (perfect forwarding).
	 - Идеальная передача позволяет создавать функции-обертки, передающие параметры без каких-либо изменений (lvalue передаются как lvalue, а rvalue – как rvalue)
		и тут std::move нам не подходит, так как она безусловно приводит свой результат к rvalue.

	 1. The move function converts an lvalue into an rvalue reference. After the conversion, the lvalue can no longer be used.
	 	
	 2. The forward function is a type of forwarding, which can continue to forward lvalue references as lvalues,
	    and rvalue references continue to be forwarded as rvalue references.	
	
============================================================================================================================================================	
					                           			Сортировки
============================================================================================================================================================

0.  Bubble Sort
	void BubbleSort(int* data, unsigned int size) {
		for (unsigned int i = 0; i < size - 1; i++) {
			for (unsigned int n = 0; n < size - i - 1; n++) {
				if (data[n] > data[n + 1])
					std::swap(data[n], data[n + 1]);
			}
		}
	}
	
	void BubbleSortEx(int* data, unsigned int size) {
		bool changed{ true };
		for (size_t i = 0; i < size && changed; i++) {
			changed = false;
			for (size_t n = 1; n < size -i; n++) {
				if (data[n - 1] > data[n]) {
					std::swap(data[n - 1], data[n]);
					changed = true;
				}
			}
		}
	}
		
	void BubbleSort(int* data, unsigned int size) {
		for (int i = size - 1; i >= 0; i--) 
			for (size_t n = size - 1; n >= size -i; n--) {
				if (data[n - 1] > data[n])
					std::swap(data[n - 1], data[n]);
			}
		}
	}
	
  -  Пузырьковая сортировка, для контейнеров, без временных переменных:
		template <typename T >
		void bubble_sort(T &a ) {
			for( T::size_type i = 0; a.size() && i < a.size() - 1; ++i ) {
				for( T::size_type j = i; j + 1 > 0; --j ) {
					if( a[j] > a[j+1] )
						std::swap( a[j], a[j+1] );
				}
			}
		}

1.  Сортировка вставками (Insertion sort)
		for (unsigned int i = 1; i < size; i++) {
			for (int j = i; j > 0 && data[j - 1] > data[j]; j--) {
				std::swap(data[j - 1], data[j]);
			}
		}
		
2. SelectionSort
		for (unsigned int min_element_index = 0, i = 0; i < size; i++) {
			min_element_index = i;
			for (unsigned int n = i; n < size; n++) {
				if (data[min_element_index] > data[n])
					min_element_index = n;
			}
			std::swap(data[i], data[min_element_index]);
		}
	
3.  QSort:
		void qsort(int* data, const int left, const int right) {
			int start = left, end = right;
			int x_element = data[(left + right) / 2];
			do {
				while (data[start] < x_element)
					start++;
				while (data[end] > x_element)
					end--;
				if (start <= end) {
					if (data[start] > data[end])
						std::swap(data[start], data[end]);
					start++;
					end--;
				}
			} while (start <= end);
			if (start < right)
				qsort(data, start, right);
			if (left < end)
				qsort(data, left, end);
		}

============================================================================================================================================================	
								                    STL Containers:
============================================================================================================================================================

0. std::string.
	   - Empty string capacity = 15
	   - Сам объект хранится на стеке а указатель на данные в куче.
	   ▪ Small String Optimization (SSO):
			std::string class, will reserve a small chunk of memory, a “small buffer” embedded inside std::string objects,
			and when strings are small enough, they will be kept (deep-copied) in that buffer, without triggering dynamic memory allocations.
			So for string less than 15 chars there’s no memory allocated on the heap

            No new[], no HeapAlloc, no virtual memory manager overhead! It’s just a simple snappy stack allocation, followed by a string copy.

1. Vector.  -----------------------------
    Сложность (эффективность) обычных операций над векторами следующая:
		Произвольный доступ — постоянная O(1)
		Вставка и удаление элементов в конце — амортизированная постоянная O(1)
		Вставка и удаление элементов — линейная по расстоянию до конца вектора O(n)
		
    When to use emplace
		To avoid copying or moving
		With unmovable types
		To ensure lifetime stability
		To get a default-constructed element: v.emplace_back().read(file);

2. 'List'
		Двусвязный список, элементы которого хранятся в произвольных кусках памяти, в отличие от контейнера vector, где элементы хранятся в непрерывной области памяти. 
		Поиск перебором медленнее, чем у вектора из-за большего времени доступа к элементу.
		Доступ по индексу за O(n).
		В любом месте контейнера вставка и удаление производятся очень быстро — за O(1).
		
3. Deque
		Двухсторонняя очередь. Контейнер похож на vector, но с возможностью быстрой вставки и удаления элементов на обоих концах за O(1). 
		Реализован в виде двусвязанного списка линейных массивов. 
		С другой стороны, в отличие от vector, двухсторонняя очередь не гарантирует расположение всех своих элементов в непрерывном участке памяти, что делает невозможным безопасное использование арифметики указателей для доступа к элементам контейнера.
		Представляет собой последовательный индексированный контейнер, который позволяет быстро вставлять и удалять элементы с начала и с конца. 
		Кроме того, вставка и удаление с обоих концов двусторонней очереди оставляет действительными указатели и ссылки на остальные элементы.
		В отличие от std::vector, элементы deque не хранятся непрерывно: обычно реализован с помощью набора выделенных массивов фиксированного размера.
		Хранилище deque обрабатывается автоматически, расширяясь и сужаясь по мере необходимости. 
		Расширение deque дешевле, чем расширение std::vector, потому что оно не требует копирования существующих элементов в новый участок памяти.
		Сложность (производительность) стандартных операций над двусторонней очередью следующая:
		- Произвольный доступ - постоянная O(1)
		- Вставка и удаление элементов с начала и с конца - амортизированная постоянная O(1)
		- Вставка и удаление элементов - линейная O(n)

4. В чем отличие vector от deque?
		Здесь вспоминают о наличии у deque методов push_front и pop_front. 
		Но основное отличие в организации памяти, у vector она как у обычного Си-массива, 
		т.е. последовательный и непрерывный набор байт, а у deque это фрагменты с разрывами. 
		За счет этого отличия vector всегда можно привести к обычному массиву или скопировать целиком участок памяти, 
		но зато у deque операции вставки/удаления в начало быстрее (O(1) против O(n)), ввиду того, что не нужно перемещать остальные значения.
			   		   
5. В чем отличие std::list<T> от std::vector<T>?
		std::vector<T> — это обертка над обычным С/C++ массивом. Соответственно:
		0. A Vector is a contiguous block of memory locations similar to Arrays in “C” programming while a List stores non-contiguous blocks and uses a double linked list.
		1. если std::vector<T> заполнен, то при добавлении нового элемента, массив удаляется целиком и создается заново с бОльшим размером
		2. любой элемент массива можно получить моментально, потому что позиция элемента вычисляется банальным прибавлением индекса к 
			первому элементу (array[i] = array + )	
		3. удаление любого элемента из массива, кроме последнего, приведет к перемещению всех элементов справа от удаляемого на одну позицию влево 
			(при соблюдении некоторых   условий, можно воспользоваться хаком) занимает неразрывный блок памяти std::list<T> — это список элементов, 
			которые связаны между собой указателями prev предыдущий элемент) и next (следующий элемент). 
			Внутри себя std::list<T> хранит указатель на первый элемент и последний (зависит от реализации). Исходя из этого:
		4. чтобы удалить любой элемент, необходимо просто переставить указатели для того, чтобы обратиться к произвольному элементу, 
			необходимо пробежаться от начала или конца (в зависимости от реализации) к искомому каждому элементу выделяется память индивидуально
		5. However sorting and searching are much faster in Vector compared to List and in former no traversal is required unlike List.

6. map vs unordered_map:
						  | map                  | unordered_map
		---------------------------------------------------------
		Ordering          | increasing  order    | no ordering
		Implementation    | Self balancing BST   | Hash Table
						  | like Red-Black Tree  |  
		search time       | log(n)               | O(1) -> Average 
						  |                      | O(n) -> Worst Case
		Insertion time    | log(n) + Rebalance   | Same as search
		Deletion time     | log(n) + Rebalance   | Same as search
		
		Use std::map when
		▪ You need ordered data.
		▪ You would have to print/access the data (in sorted order).
		▪ You need predecessor/successor of elements.
		▪ See advantages of BST over Hash Table for more cases.
		
		How unordered_map store elements?
		The reason because unordered_map does the hashing i.e. whenever we try to insert an element in a unordered_map, it internally does the following steps,
		▪ First hash of key is calculated using Hasher function and then on the basis of that hash an appropriate bucket is choose.
		▪ Once bucket is identified then it compares the key with key of each element inside the bucket using Comparator function to identify if given element is a duplicate or not.
		▪ If its not a duplicate then only it stores the element in that bucket.
		  Therefore, there is no specific order in which elements are stored internally.
		
		Advantage of Unordered_map
		▪ The basic advantage of using unordered_map instead of associative map is the searching efficiency. 
		  In an unordered_map complexity to search for an element is O(1) if hash code are chosen efficiently.


7. List vs Forward_List:
		The main design difference between a forward_list container and a list container is that the first keeps internally only a link to the next element, while the latter keeps two links per element: one pointing to the next element and one to the preceding one, allowing efficient iteration in both directions, but consuming additional storage per element and with a slight higher time overhead inserting and removing elements. 
		forward_list objects are thus more efficient than list objects, although they can only be iterated forwards.
		

8. std::map.
      Есть ли разница между map::operator[] и  insert();
      Функция map::operator[] упрощает операции «обновления с возможным созданием». 
		Иначе говоря, при наличии объявления map<K, V> m команда m[k]=v; проверяет, присутствует ли ключ k в контейнере.
		Если ключ отсутствует, он добавляется вместе с ассоциированным значением v. Если ключ уже присутствует, ассоциированное с ним значение заменяется на v.
		Для этого operator[] возвращает ссылку на объект значения, ассоциированного с ключом k, после чего v присваивается объекту, к которому относится эта ссылка. 
		При обновлении значения, ассоциированного с существующим ключом, никаких затруднений не возникает — в контейнере уже имеется объект, ссылка на который возвращается функцией operator[]. 
		Но при отсутствии ключа k готового объекта, на который можно было бы вернуть ссылку, не существует.
		В этом случае объект создается конструктором по умолчанию, после чего operator[] возвращает ссылку на созданный объект.

   Deque vs. Vector
	• Use a vector if the container is small
	• Pushing onto the front of a vector is faster until container size is surprisingly big
	• Vector is much smaller for small containers
	• Use a deque when the container may get large
		
   List vs. Vector
	• Use a vector if the container is small
	• Inserting anywhere in a vector is faster until container size is surprisingly big
	• Vector is much, much smaller for any size(but has the growth problem)
	• Use a list when the container is large and you are doing lots of inserts and/or deletes in the middle
	• Use a list when you can benefit from splicing
	
   Set/Map vs. Vector
	• Use a vector if the container is small
	• Arbitrary insertion is faster until the size is quite large
	• Maintaining order is faster until the size is quite large
	• Binary search on vector works just as well
	• For smallish containers, linear search is faster
	• Pre-reserve and the vector will not move
	• Use a set or map when the container is large
		
============================================================================================================================================================	
								                    BOOST
============================================================================================================================================================	
	
1. boost::circular_buffer<int> circular_buffer - Very fast!!!
	[в случае если Клнсюмер успевает за Продюсером]
		
		
============================================================================================================================================================	
								                    Data Structures:
============================================================================================================================================================		
	
1. Self-balancing binary search tree	
		In computer science, a self-balancing (or height-balanced) binary search tree is any node-based binary search tree that 
		automatically keeps its height (maximal number of levels below the root) small in the face of arbitrary item insertions and deletions.
		
		Most operations on a binary search tree (BST) take time directly proportional to the height of the tree, so it is desirable to keep the height small. 
		A binary tree with height h can contain at most 2 pow(h+1) − 1 nodes. 
		It follows that for any tree with n nodes and height h:	
		
1. 	Balanced Binary Tree:
		A binary tree is balanced if the height of the tree is O(Log n) where n is the number of nodes.
		For Example, the AVL tree maintains O(Log n) height by making sure that the difference between the heights of the left and right subtrees is almost 1. 
		Red-Black trees maintain O(Log n) height by making sure that the number of Black nodes on every root to leaf paths is the same and there are no adjacent red nodes.
		Balanced Binary Search trees are performance-wise good as they provide O(log n) time for search, insert and delete.

2. 	Full Binary Tree:
		Binary Tree is a full binary tree if every node has 0 or 2 children. The following are the examples of a full binary tree.
		We can also say a full binary tree is a binary tree in which all nodes except leaf nodes have two children.
							  18
						   /    \   
						 15     20    
						/  \       
					  40    50   
					/   \
				   30   50
		
3. 	Complete Binary Tree:
		A Binary Tree is a complete Binary Tree if all the levels are completely filled except possibly the last level and the last level has all keys as left as possible
						  18
					   /      \  
					 15        30  
					/  \      /  \
				  40    50  100    40
				 /  \   /
				8   7  9 
			
4. 	Perfect Binary Tree:
		Binary tree is a Perfect Binary Tree in which all the internal nodes have two children and all leaf nodes are at the same level.
						   18
						 /    \  
					   6       9  
					 /  \     /  \
				   2     4   5     8
				   
5. 	Array:

6. 	Stack: FILO

7.  Queue: FIFO

8.  Lists

9.  Graphs

10. Trees

11. Heap

12. Hash Tables

13. Map

======================================================= Big O: =============================================================

На самом деле, существует на только <Big O>, но и ряд других обозначений. Рассмотрим их:
	O(ƒ(n)) – (Big-O)    – верхняя граница, «не хуже чем»
	o(ƒ(n)) – (Little-o) – верхняя граница, «лучше чем»
	Ω(ƒ(n)) – (Omega)    – нижняя граница,  «не лучше чем»
	Θ(ƒ(n)) – (Theta)    – точная оценка.

	▪ Logarithmic algorithm – O(logn)
	  [Runtime grows logarithmically in proportion to n.]
	  - Binary Search.

	▪ Linear algorithm – O(n)
	  [Runtime grows directly in proportion to n.]
	  - Linear Search.
	
	▪ Superlinear algorithm – O(nlogn)
	  [Runtime grows in proportion to n.]
	  - Heap Sort, Merge Sort.
	
	▪ Polynomial algorithm – O(n^c)
	  [Runtime grows quicker than previous all based on n.]
	  - 	Strassen’s Matrix Multiplication, Bubble Sort, Selection Sort, Insertion Sort, Bucket Sort.
	
	▪ Exponential algorithm – O(c^n) – Tower of Hanoi.
	Runtime grows even faster than polynomial algorithm based on n.
	
	▪ Factorial algorithm – O(n!) – Determinant Expansion by Minors, Brute force Search algorithm for Traveling Salesman Problem.


================================================================== C++ Type casts: ==================================================================

	▪ static_cast: Is the first cast you should attempt to use. It does things like implicit conversions between types (such as int to float, or pointer to void*), 
			and it can also call explicit conversion functions (or implicit ones). 
			In many cases, explicitly stating static_cast isn't necessary, but it's important to note that the T(something) syntax is equivalent to (T)something and should be avoided (more on that later). A T(something, something_else) is safe, however, and guaranteed to call the constructor.

			it can also cast through inheritance hierarchies. It is unnecessary when casting upwards (towards a base class), 
			but when casting downwards it can be used as long as it doesn't cast through virtual inheritance. 
			It does not do checking, however, and it is undefined behavior to static_cast down a hierarchy to a type that isn't actually the type of the object.
			
			static_cast is also able to perform all conversions allowed implicitly (not only those with pointers to classes), and is also able to perform the opposite of these. 
			It can:
			- Convert from void* to any pointer type. In this case, it guarantees that if the void* value was obtained by converting from that same pointer type, the resulting pointer value is the same.
			- Convert integers, floating-point values and enum types to enum types.

			Additionally, static_cast can also perform the following:
			- Explicitly call a single-argument constructor or a conversion operator.
			- Convert to rvalue references.
			- Convert enum class values into integers or floating-point values.
			- Convert any type to void, evaluating and discarding the value.

	▪ const_cast. Can be used to remove or add const to a variable; no other C++ cast is capable of removing it (not even reinterpret_cast). 
			It is important to note that modifying a formerly const value is only undefined if the original variable is const;
			if you use it to take the const off a reference to something that wasn't declared with const, it is safe. 
			This can be useful when overloading member functions based on const, for instance. 
			It can also be used to add const to an object, such as to call a member function overload.

			also works similarly on volatile, though that's less common.

	▪ dynamic_cast. is exclusively used for handling polymorphism. 
			You can cast a pointer or reference to any polymorphic type to any other class type (a polymorphic type has at least one virtual function, declared or inherited). You can use it for more than just casting downwards – you can cast sideways or even up another chain. 
			The dynamic_cast will seek out the desired object and return it if possible. 
			If it can't, it will return nullptr in the case of a pointer, or throw std::bad_cast in the case of a reference.

			It has some limitations, though. It doesn't work if there are multiple objects of the same type in the inheritance hierarchy (the so-called 'dreaded diamond') and you aren't using virtual inheritance. 
			It also can only go through public inheritance - it will always fail to travel through protected or private inheritance. 
			This is rarely an issue, however, as such forms of inheritance are rare.
					
			You only need a dynamic_cast when you have a derived class that defines something different that's not present in the base class, 
			and you need/want to take the extra something into account

	▪ reinterpret_cast is the most dangerous cast, and should be used very sparingly. 
			It turns one type directly into another — such as casting the value from one pointer to another, or storing a pointer in an int, or all sorts of other nasty things. 
			Largely, the only guarantee you get with reinterpret_cast is that normally if you cast the result back to the original type, you will get the exact same value (but not if the intermediate type is smaller than the original type). There are a number of conversions that reinterpret_cast cannot do, too.
			It's used primarily for particularly weird conversions and bit manipulations, like turning a raw data stream into actual data, or storing data in the low bits of a pointer to aligned data.
			
  -  Что такое Upcasting and Downcasting:
			Upcasting: Derived --> Base
			           Upcasting is a process of treating a pointer or a reference of derived class object as a base class pointer. 
					   You do not need to upcast manually. You just need to assign derived class pointer (or reference) to base class pointer
			Downcasting: Downcasting is an opposite process for upcasting. It converts base class pointer to derived class pointer. 
					     Downcasting must be done manually. It means that you have to specify explicit type cast.
					
============================================================================================================================================================	
								                                  decltype(auto) vs auto | Return type
============================================================================================================================================================  

The decltype(auto) type-specifier also deduces a type like auto does. 
However, it deduces return types while keeping their references and cv-qualifiers, while auto will not.


const int x = 0;                          |  int y = 0;                             |   int&& z = 0;
auto x1 = x;           /** int **/        |  int& y1 = y;                           |   auto z1 = std::move(z);           /** int   **/ 
decltype(auto) x2 = x; /** const int **/  |  auto y2 = y1;           /** int  **/   |   decltype(auto) z2 = std::move(z); /** int&& **/ 
										  |	 decltype(auto) y3 = y1; /** int& **/   | 

============================================================================================================================================================	
								                                  Smart pointers:
============================================================================================================================================================  

 ▪ std::unique_ptr:
	 -  get():    The stored pointer points to the object managed by the unique_ptr, if any, or to nullptr if the unique_ptr is empty.
	 -  release() Releases ownership of its stored pointer, by returning its value and replacing it with a null pointer. 
	 -  reset():  Destroys the object currently managed by the unique_ptr (if any) and takes ownership of p.
	 
	 - есть ли разница при замене значение unique_ptr указателя с помощью std::move или reset(prt.release())
	   - ДА. release не вызывает делетер
	         Возможны как утечки памяти (в случае c custom deleter) так и undefined hehavior.
			 TODO: изучить
			 
	 -  Вот тут при разрушении new Integer(222) все равно будет вызван делетер IntegerDeleter():
	         std::unique_ptr<Integer, IntegerDeleter> up(new Integer(111), IntegerDeleter());
	         up.reset(new Integer(222));
						 
	 -  Вот тут вообще не будет вызваны DELETER-ы - то есть учтечка потенциальная!!!
			std::unique_ptr<Integer, IntegerDeleter> int1(new Integer(111), IntegerDeleter());
			std::unique_ptr<Integer> int2 = std::make_unique<Integer>(222);
			int2.reset(int1.release());

	 -  А вот тут будут вызваны DELETER-ы для обоих объектов:
			std::unique_ptr<Integer, IntegerDeleter> int1(new Integer(111), IntegerDeleter());
			std::unique_ptr<Integer> int2 = std::make_unique<Integer>(222);
			int1.reset(int2.release());


	 -  // C-style file management
    	std::unique_ptr<FILE, decltype(&fclose)> file(fopen("foo.txt", "r"), fclose);

	 -  // Works on malloc/free too, pass a deleter argument
    	std::shared_ptr<Foo> bar(function_calling_malloc(), free);


 ▪ std::shared_ptr:	-----------------------------------------------------
 
	-  std::enable_shared_from_this: 
	   позволяет объекту t, который управляется умным указателем pt с типом std::shared_ptr, безопасно создать дополнительные экземпляры pt1, pt2, ... 
	   с типом std::shared_ptr, такие, что все они будут раздельно владеть объектом t, наравне с pt.
		   
	-  можно безопасно работать с этим shared_ptr в многопоточной среде ?
	   НЕТ. !!!
	   Вывод: если какой-то экземпляр shared_ptr доступен разным потокам и может быть модифицирован, то
	   необходимо позаботиться о синхронизации доступа к этому экземпляру shared_ptr.
 
	-  можно ли вызывать shared_from_this() в конструкторе
	   НЕТ. !!!
	   The reason is simple: in object X, enable_shared_from_this works by initialising a hidden weak_ptr with a copy of the first shared_ptr which points to object X. However, for a shared_ptr to be able to point to X, X must already exist (it must be already constructed). 
	   Therefore, while the constructor of X is running, there is yet no shared_ptr which enable_shared_from_this could use.
	   
	  ▪ std::shared_ptr<T> *******[Pros and cons] ******
	   
	   1. std::make_shared<T> typically performs only ONE allocation
	      [while std::shared_ptr<T>(new T(args...)) performs at least two allocations (one for the object T and one for the control block of the shared pointer)]
		  
	   2. Order of execution and exception safety

	   3. std::make_shared does not allow a CUSTOM DELETER.
	   
	   4. std::make_shared requires public access to the selected constructor.
		  [while std::shared_ptr<T>(new T(args...)) may call a non-public constructor of T if executed in context where it is accessible,]
	
		  class Bar {
				Bar() = default;
		  public:
				static std::shared_ptr<Bar> create() 
					// return std::make_shared<Bar>(); // ERROR !!!!!!!!! ---------- Это не компилится (((
					return std::shared_ptr<Bar>(new Bar);
				}
		  };	
		
	-  Нужно быть осторожным при использовании контейнеров умных указазателей с применением emplace и emplace_back методов
	   Так как если во времени обработки аргуметов переданых в emplace метод случится исключение, и один из переданых аргументов к примеру
	   созданный с помощью new объект. то так как при вызове emplace_back сам объект умного указателя shared_ptr создается позже (в отличае push_back)
	   то и не будет вызват delete для объекта в куче - что означает exception safely нарушается.

 ▪ std::weak_ptr:	-----------------------------------------------------

	-  Use std::weak_ptr for std::shared_ptr-like pointers that can dangle.
	-  Potential use cases for std::weak_ptr include caching, observer lists, and the prevention of std::shared_ptr cycles.

	Constructing a Shared Pointer from a Weak Pointer:
	-	A shared pointer can be constructed from a weak pointer
	-	Should you attempt to construct a shared pointer from a dangling weak pointer, a std::bad_weak_ptr exception is raised.

		auto sp = std make_shared<Widget>();  // Create a shared pointer
		std weak_ptr<Widget> wp(sp);          // Create a weak pointer from the shared pointer
		bool is_dangling = wp.expired();      // Check if the pointer is dangling
		std shared_ptr sp_of_wp = wp.lock();  // Retrieve the original shared pointer the weak pointer was derived from

	Is std::weak_ptrT::lock thread-safe
	-   YES:  weak_ptr::lock is "executed atomically" 'https://cplusplus.github.io/LWG/issue2316'


 ▪ Storage of custom deleters -----------------------------------------------------
	-  For shared_ptr
		When you use a custom deleter it won’t affect the size of your shared_ptr type.
		Shared_ptr size should be roughly 2 x sizeof(ptr) so where does this deleter hide
		As we know, shared_ptr consists of two things: pointer to the object and pointer to the control block (that contains ref count). 
		Inside the control block structure of shared_ptr, there is a space for custom deleter and allocator.

	-  unique_ptr
		unique_ptr is small and efficient; the size is one pointer so where is the custom allocator hide in this case?
		The deleter is part of the type of unique_ptr. And since the functor/lambda that is stateless, 
		its type fully encodes everything there is to know about this without any size involvement. 
		Using function pointer takes one pointer size and std::function takes even more size.

		*** Deleter для std::unique_ptr<T> обязан задавать в compile-time ****

	   
============================================================================================================================================================
							 𝗛𝗶𝗱𝗱𝗲𝗻 𝗠𝗲𝗺𝗼𝗿𝘆 𝗖𝗼𝘀𝘁𝘀 𝗶𝗻 𝗖++: 𝗨𝘀𝗶𝗻𝗴 𝘀𝘁𝗱::𝗺𝗮𝗸𝗲_𝘀𝗵𝗮𝗿𝗲𝗱 𝘄𝗶𝘁𝗵 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿:
============================================================================================================================================================

When you create a shared pointer using 𝘀𝘁𝗱::𝗺𝗮𝗸𝗲_𝘀𝗵𝗮𝗿𝗲𝗱, everything seems efficient - just one allocation for the object and its control block.
But if you then store a 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿 pointing to this shared pointer, the control block has to remain in memory, even after all shared pointers are destroyed. Why?
The weak counter inside the control block keeps it alive!
Although the object itself gets destructed, 𝘀𝘁𝗱::𝗺𝗮𝗸𝗲_𝘀𝗵𝗮𝗿𝗲𝗱 makes one allocation, thus one deallocation is expected, leaving the memory which contains the object reserved.

Example 1:

	Using 𝘀𝘁𝗱::𝗺𝗮𝗸𝗲_𝘀𝗵𝗮𝗿𝗲𝗱 with 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿 leaves memory reserved because the single allocation includes space for the weak reference counter,
	so the whole memory keeps being reserved until 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿-s get destructed. Watch how the memory usage continues to grow.

Example 2:

	Using 𝗻𝗲𝘄 with 𝘀𝘁𝗱::𝘀𝗵𝗮𝗿𝗲𝗱_𝗽𝘁𝗿 gives you more explicit control over when memory is deallocated,
	which can improve memory management in cases where 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿 significantly outlives the 𝘀𝘁𝗱::𝘀𝗵𝗮𝗿𝗲𝗱_𝗽𝘁𝗿.

Pro Tip:

	If your application heavily uses 𝘀𝘁𝗱::𝘄𝗲𝗮𝗸_𝗽𝘁𝗿 which outlives 𝘀𝘁𝗱::𝘀𝗵𝗮𝗿𝗲𝗱_𝗽𝘁𝗿, consider the implications of using 𝘀𝘁𝗱::𝗺𝗮𝗸𝗲_𝘀𝗵𝗮𝗿𝗲𝗱 vs. 𝗻𝗲𝘄.
	Understanding these subtleties can help prevent memory issues in performance-critical applications.

============================================================================================================================================================	
								                                    Templates:
============================================================================================================================================================  
	 
 ▪ Fold expression exmaples:
 
  	template<typename ...Args> 
	auto sum(Args ...args) {
		return (args + ... + 0);
	}

	template<typename ...Args>
	auto sum2(Args ...args) {
		return (args + ...);
	}

	template<typename ...Args>
	void FoldPrint(Args&&... args) {
		(std::cout << ... << std::forward<Args>(args)) << std::endl;
	}
	
	template<typename T, typename... Args>
	void push_back_vec(std::vector<T>& vector, Args&&... args) {
		(vector.push_back(args), ...);
	}

	template<typename FunPtr, typename ...Args>
	void invoke(FunPtr funcPtr, Args&& ...args) {
		(funcPtr(args), ...);
	}
	
	template<class ...Args>
	bool invokeFuncs(Args ... args) {
		return (args() && ...);
	}
	
	
	template<typename... Bases>
	class MultiBase : private Bases... {
	public:
		void print() { 
			// call print() of all base classes:
			(..., Bases::print());
		}
	};
	
 ▪ Folded Function Calls
 
 	template<typename... Bases>
	struct MultiBase : private Bases... {
		void print() {  // call print() of all base classes:
			(..., Bases::print());
		}
	};

	struct A { void print() { std::cout << "A::print()\n"; } };
	struct B { void print() { std::cout << "B::print()\n"; } };
	struct C { void print() { std::cout << "C::print()\n"; } };

	void FoldClassMethod() {
		MultiBase<A, B, C>().print();
	}
  
 ▪ Variadic lambda:
 
	auto foo = [](auto... param) {
		return Sum(param...);
	};
	
 ▪ Curiously recurring template pattern:
	Bдиома языка C++, название которой можно примерно перевести как Странно рекурсивный шаблон или Странно повторяющийся шаблон
	Суть в том, что некоторый класс X наследуется от шаблона класса, использующего X как шаблонный параметр.
	Используется и в Java — например, любой enum X является наследником от Enum<X>.
	
	// The Curiously Recurring Template Pattern (CRTP)
	template<class T>
	class Base { // methods within Base can use template to access members of Derived
	};
	class Derived : public Base<Derived> { /** **/ };
	

 ▪  //================================== Specialisation =================================
	
    template<class T>
    struct TestClass {
        TestClass() { std::cout << "General template object" << std::endl; }
    };

    template<>
    struct TestClass<int> {
        TestClass() { std::cout << "Specialized for INT\n"; }
    };

    template<>
    struct TestClass<std::string> {
        TestClass() { std::cout << "Specialized for STRING\n"; }
    };

	
 ▪  //================================== Partial specialisation =================================
	
	template<typename T>
	struct is_pointer : std::false_type {};

	template<typename T>
	struct is_pointer<T*> : std::true_type {};
	
	/////
	
     template<typename T1, typename T2>
    struct Object {
        Object() { std::cout << "Object<T1, T2>()" << std::endl; }
    };

    template<typename T>
    struct Object<T, T> {
        Object() { std::cout << "Object<T, T>()" << std::endl; }
    };


    template<typename T>
    struct Object<T, int> {
        Object() { std::cout << "Object<T, int>()" << std::endl; }
    };

    template<typename T1, typename T2>
    struct Object<T1 *, T2 *> {
        Object() { std::cout << "Object<T1*, T2*>()" << std::endl; }
    };

    void Test() {
        Object<int, float> mif;    // uses Object<T1, T2>()
        Object<float, float> mff;  // uses Object<T , T>()
        Object<float, int> mfi;    // uses Object<T , int>()
        Object<int *, float *> mp; // uses Object<T1*,T2*>()
    }

	
 ▪  //================================== Frient template class =================================


    template<typename T = std::string>
    class Credentials {
        T password {};

        template<typename M>
        friend class Handler; // every Handler<M> is a friend of A --> can access 'password'
    };

    template<typename T>
    class Handler {
        std::vector<Credentials<T>> secrets {};

    public:
        void process() {
            std::for_each(secrets.cbegin(), secrets.cend(), [](const auto& s) { auto x = s.password; });
        }
    };


============================================================================================================================================================	
								                                    Сoncepts:
============================================================================================================================================================  

 ▪  The three valid places of requires
 	 1. /** As the requires clause: **/

 	   template<typename T>
 	   requires YourRequirementOrConcept<T>
 	   void func();

 	 2. // As the trailing-requires-clause:

 	   template<typename T>		
 	   void func() requires YourRequirementOrConcept<T>;

 	 3. // As a constrained template parameter:

 	   template<YourRequirementOrConcept T>
	   void func();



 ▪  template<typename T>
    concept Comparable = requires(T const& a, T const& b) {
        {a < b} -> std::same_as<bool>;
        {a > b} -> std::same_as<bool>;
    };
	 // NOTE: -> std::same_as<bool> can be skiped
    // the expression {a > b} must be valid  AND std::same_as<decltype((a > b)), bool>
    // must be satisfied i.e., (a > b) must return 'bool'

	template<Comparable T>
    void compare(T a, T b) {
    }
	
 ▪  // Проверка что это Container и он имеет некие методы

 	   template<typename T>
 	   concept Container = requires(T t)
 	   {
	        typename T::value_type;
	        typename T::size_type;
	        typename T::allocator_type;
	        typename T::iterator;
	        typename T::const_iterator;

	        t.size();
	        t.begin();
	        t.end();
	        t.cbegin();
	        t.cend();
 	   };
	
 ▪  // Проверка что входные параметры метода foo позволяют создать объект типа Т 
	
	template<typename T, typename... Args>
    concept Constructible = requires(Args&&... params) { 
        { T(std::forward<Args>(params)...) };
    };

    template<typename T, typename... Args>
    requires Constructible<T, Args...>
    void foo(T a, Args... params) {
    }
	
	// Если объект момжо создать std::is_constructible_v<Concrete, Ts...> то создаем, в ином случае --> nullptr
	
	template <typename Concrete, typename... Ts>
    std::unique_ptr<Concrete> constructArgsOne(Ts&&... params) {
        if constexpr (std::is_constructible_v<Concrete, Ts...>)
            return std::make_unique<Concrete>(std::forward<Ts>(params)...);
        else
            return nullptr;
    }
	
 ▪  // Fold expression && Concetps
		
    template<typename ... Types>
    concept TestConcept = requires(Types ... params) {
        (... + params);
        // requires std::sam
        // requires (sizeof ... (params)) > 1;
        // { (... + params) } noexcept -> std::same_as<first_arg_t<Types ...>>;
    };
	
	template<std::predicate<int>... Func>
    void check_with_params_varidic_1(Func... func) {
        (func(123), ...);
    }
	
 ▪  // Check for now throw
	
	template <typename T>
    concept cant_throw_for_some_operation = requires (T a, T b) {
        { a = b } noexcept;
		{ a.swap(b) } noexcept;
    };
	
	template <typename T>
    concept NoExceptDestructible = requires (T & value) {
        { value.~T() } noexcept;
    };
	
	template <typename T>
    concept NoExceptDereference = requires (T & value) {
        {*x} noexcept;  // dereference must be noexcept
    };
	
	template<typename Func>
    concept NoexceptFunc = requires (Func f) {
        { f() } noexcept;              // requires Func not to throw any exception
        requires std::invocable<Func>; // requires Func type to be callable:
    };
	
 ▪  // Проверка наличия у класса членов
	
	template<typename T>
    concept has_string_data_member = requires(T v) {
        { v.name } -> std::convertible_to<std::string>;
		{ v.size()} -> std::same_as<size_t>;
		 typename T::ElementType; // ElementType member type must exist
    };
	
 ▪  // Сложный консепт
	
    template<typename T>
    concept Addable = requires(T a, T b) {
        a + b;
    };

    template<typename T>
    concept Dividable = requires(T a, T b) {
        a / b;
    };

    template<typename T>
    concept SomeComplexConcept = requires (T x) // optional set of fictional parameter(s) {   
        x++;                                                    // simple requirement: expression must be valid expression must be valid
        typename T::value_type;                                 // type requirement: `typename T`, T type must be a valid type
        // typename S<T>;
        // compound requirement: {expression}[noexcept][-> Concept];
        // {expression} -> Concept<A1, A2, ...> is equivalent to
        // requires Concept<decltype((expression)), A1, A2, ...>
        {*x};  													// dereference must be valid
        {*x} noexcept;  										// dereference must be noexcept
        {*x} noexcept -> std::same_as<typename T::value_type>;  // dereference must  return T::value_type
        requires Addable<T>;    								// nested requirement: requires ConceptName<...>; constraint Addable<T> must be satisfied
        requires Dividable<T>;  								// nested requirement: requires ConceptName<...>; constraint Dividable<T> must be satisfied
        { x.~T() } noexcept;
    };
  
============================================================================================================================================================	
								                                    Exceptions:  https://www.youtube.com/watch?v=Oy-VTqz1_58
============================================================================================================================================================  

  - Для чего используется вызов throw без аргументов?
 		Ответ: Для повторного возбуждения предыдущего исключения и направления его следующему обработчику.

 -  Что стоит помнить при использовании исключений в конструкторе объекта?
		Ответ: Если исключение не обработано, то c логической точки зрения разрушается объект, который еще не создан, а с технической, так как он еще не создан, то и деструктор этого объекта не будет вызван.
  
  - Как сгенерировать pure virtual function call исключение
		Ответ: Нужно вызвать чисто виртуальный метод в конструкторе родительского класса т.е. до создания дочернего, в котором этот метод реализован. 
			   Т.к. современный компилятор не даст это сделать напрямую, то нужно будет использовать промежуточный метод.
			   
  - Будет ли вызван деструктор класса если в его конструкторе было выброшено исключение:
		Ответ: Нет
		
  - Будет ли вызван деструктор БАЗОВОГО класса если в конструкторе ПОРОЖДЕННОГО класса было выброшено исключение:
		Ответ: деструктор БАЗОВОГО класса - да
		       деструктор ПОРОЖДЕННОГО класса - нет	   
			   
  - Будет ли вызван деструктор класса если в его конструкторе было выброшено исключение НО этот констуктор до этого вызвал успешного  
    другой конструктор этого класса
		Ответ: ДА. Будет
	  
		class Base {
		public:
			Base(): Base("TEst") { throw std::runtime_error("Exception from Base_WithException::Base_WithException()"); }
			Base(const std::string& s) { std::cout << "Base::Base(const std::string& s)" << std::endl; }
			virtual ~Base() noexcept(false) { std::cout << "Base::~Base()" << std::endl;}
		};
			   	
  - Как отловить исключение выброшенного при инициализации одного из членов класса до конструктора:
		class TestClass {
		private:
			A a; 
			B b; // It throws somethingv
			C c;

		public:
			TestClass() try {
				std::cout << "Code of TestClass Ctor" << std::endl;
			} catch (const std::exception& exc) {
				std::cout << "Here we will catch exception& from class B " << exc.what() << std::endl;
			}
		
			virtual ~TestClass() { std::cout << "Dtor" << std::endl; }
		};
		
  - What is the Function-try-block:
		Ответ: The function-try-block is one of the alternative syntax forms for function-body, which is a part of function definition.
			   The primary purpose of function-try-blocks is to respond to an exception thrown from the member initializer list in a constructor by logging and rethrowing,
			   modifying the exception object and rethrowing, throwing a different exception instead, or terminating the program. 
			   They are rarely used with destructors or with regular functions.
		
		struct S {
			std::string m;
			S(const std::string& arg) try : m(arg, 100) {
					std::cout << "constructed, mn = " << m << '\n';
				} catch(const std::exception& e) {
					std::cerr << "arg=" << arg << " failed: " << e.what() << '\n';
			} 
		};

  - Применение std::exception_ptr и std::current_exception()
    Для обработки/проброски исключений из потока:
	
		std::vector<std::exception_ptr> g_exceptions;
		
		void thread_func2() {
			try { func2();}
			catch (...) {
				std::lock_guard<std::mutex> lock(g_mutex);
				g_exceptions.push_back(std::current_exception());
			}
		}
		
  - Как обявиться функцию что она бросает исключение если bla-bla-bla
		// whether foo is declared noexcept depends on if the expression T() will throw any exceptions
		template <class T>
		void foo() noexcept(noexcept(T())) {}


  - Исключния и std::future / std::async

  		auto f = std::async(std::launch::async, funcWithException);
		f.wait(); ---> 
		f.get();  --->  можно поймать исключение в потоке что вызвал get()


		
  - Можно ли (стоит ли)	выбрасывать исключения из дестукторa:
  		Ответ: Нет. Не нужно так. При раскрутке стека все упадёт.

    struct demo {                                         struct demo {                                                struct base {
        ~demo() { throw std::exception{}; }                  ~demo()  noexcept(false)  { throw std::exception{}; }        ~base() noexcept(false) { throw 1; }
    };                                                    }                                                            };

    try {                                                 try {                                                        struct derive : base {
        demo d;                                           	 demo d;                                                   	   ~derive() noexcept(false) { throw 2; }
    }  catch (const std::exception &) {                   } catch (const std::exception &) {                           };
    	/** THIS TERMINATES!!!!  **/ 						  std::cerr << "Exception()\n"; 
    }                                                     }                                                            try {
    																												   		derive d;
                                                                                                                       }  catch (...) { 
                                                                                                                       	  /** CRUSH ON STACK ONWINDEING **/ 
                                                                                                                       }
  
============================================================================================================================================================	
								                               VTable [таблица виртуальных методов]
============================================================================================================================================================  


Каждому классу, использующему виртуальные функции (или производному от класса, использующего виртуальные функции), предоставляется собственная виртуальная таблица. 
Эта таблица представляет собой просто статический массив, который компилятор создает и заполняет во время компиляции.

грубо говоря в каждом классе создаётся статическое поле --> указатель на таблицу виртуальных функций для данного класса (не объекта - а класса)

Виртуальная таблица содержит по одной записи для каждой виртуальной функции, которая может быть вызвана объектами класса.
Очередность следования функий в VTable (фиксировна на моменте комплияции -- часть ABI )

Каждая запись в этой таблице – это просто указатель на функцию, указывающий на наиболее производную версию функции, доступную для данного класса.

Компилятор также добавляет скрытый указатель, который является членом базового класса, 
который мы назовем *__vptr. *__vptr устанавливается (автоматически) при создании экземпляра класса, и он указывает на виртуальную таблицу для этого класса.

В отличие от указателя *this, который на самом деле является параметром функции, используемым компилятором для вычисления ссылок на себя, *__vptr является реальным указателем. 
Следовательно, он увеличивает размер каждого размещаемого объекта класса на размер одного указателя. 
Это также означает, что *__vptr наследуется производными классами, что важно.

Чтобы вызвать виртуальную функцию, среда выполнения должна сначала получить ее адрес. Она не знает ее адреса, но ей известно смещение в виртуальной таблице, содержащей адрес. 
RunTime знает что адрес виртуальной функции 'function_0' находится со смещением 0, адрес виртуальной функции 'function_1'  со смещением 1 и т.д. 
RunTime обращается к 'vptr', чтобы найти адрес виртуальной таблицы, добавляет смещение функции, которую она хочет вызвать, получает адрес функции, а затем выполняет виртуальный вызов.


// Посмотреть как выглдяит таблица можно так:
//> g++-14.2 -fdump-lang-class -g VTableDump.cpp
//> nano a-VTableDump.cpp.001l.class

			
• What are VTABLE and VPTR?

		Ответ:  VTABLE is a table of function pointers. It is maintained per class.
				VPTR is a pointer to vtable. It is maintained per objec
		Compiler adds additional code at two places to maintain and use vtable and vptr.
		1) Code in every constructor. This code sets vptr of the object being created. This code sets vptr to point to vtable of the class.
		2) Code with polymorphic function call .  
		   Wherever a polymorphic call is made, compiler inserts code to first look for vptr using base class pointer or reference 
		   (In the above example, since pointed or referred object is of derived type, vptr of derived class is accessed). 
		   Once vptr is fetched, vtable of derived class can be accessed.
		   Using vtable, address of derived derived class function show() is accessed and calle

• How are virtual functions implemented in C++?

     	Ответ:  Virtual functions are implemented using a table of function pointers. It is called the vtable.
				There exists one entry in the table per virtual function in the class.
				The table is created by the constructor of the class.
				When a derived class is constructed, its base class is constructed first. This creates the vtable.
				The derived class may override some of the base classes virtual functions. 
				Such entries in the vtable are overwritten by the derived class constructor. 
				For this reason, the virtual functions should not be called from a constructor.


============================================================================================================================================================	
								                                    OOP:
============================================================================================================================================================  

 -  Copy Constructor
		Answer: A copy constructor is a member function which initializes an object using another object of the same class. 
				A copy constructor has the following general function prototype: ClassName (const ClassName &old_obj);

 -  What is a COPY CONSTRUCTOR and when is it called?
		Answer: 1. When an object of the class is returned by value.
				2. When an object of the class is passed (to a function) by value as an argument.
				3. When an object is constructed based on another object of the same class.
				4. When the compiler generates a temporary object.

  -  In which statement assignment operator will be called?
				A Obj1(5);
				A Obj2(Obj1);
				A Obj3 = Obj1	
     		Answer: None, for every statement copy constructor will get called.		

  - What is Copy elision: 
    	Ответ: Under the following circumstances, the compilers are required to omit the copy and move construction of class objects, 
		       even if the copy/move constructor and the destructor have observable side-effects.
			   The objects are constructed directly into the storage where they would otherwise be copied/moved to. 
			   The copy/move constructors need not be present or accessible:
			   1. In a return statement, when the operand is a prvalue of the same class type (ignoring cv-qualification) as the function return type
			   2. In a return statement, when the operand is the name of a non-volatile object with automatic storage duration, which isn't a function parameter or a catch clause parameter, and which is of the same class type (ignoring cv-qualification) as the function return type.
			   This variant of copy elision is known as NRVO, "named return value optimization".
			   3 . In a throw-expression, when the operand is the name of a non-volatile object with automatic storage duration, which isn't a function parameter or a catch clause parameter, and whose scope does not extend past the innermost try-block (if there is a try-block).
	
  - What is RVO and NRVO? What potential issues does NRVO introduce? Are there any special considerations for facilitating the compiler?
    	Ответ: RVO: An in-line constructed return value can be returned from a function by value directly into the memory space allocated to the 
		       object to which it will be returned. This is an optimization that ensures the compiler doesn't need to take a copy of a type being returned by value. 
			   The possible issue is that the copy constructor won't be called, so if your code relies on this you may get unexpected results.
			   // Example of RVO
			   Bar Foo() {
				   return Bar();
			   }
  
			   NRVO: Named Return Value Optimization. Same as above.
			   // Example of NRVO
			   Bar Foo() {
				   Bar b;
				   return b;
			   }
			   
			   When value optimization (NRVO) is used, which still requires copy/move support !!!!!!!!!!!

 -  Зачем нужен виртуальный деструктор?
		Ответ: Чтобы избежать возможной утечки ресурсов или другого неконтролируемого поведения объекта, в логику работы которого включен вызов деструктора.
		        
			   Уничтожение объекта производного класса через указатель на базовый класс с невиртуальным деструктором дает неопределенный результат. 
			   На практике это выражается в том, что будет разрушена только часть объекта, соответствующая базовому классу. 
			   Если в коде выше убрать ключевое слово virtual перед деструктором базового класса, то вывод программы будет уже иным. 
			   Обратите внимание, что член данных obj класса Derived также не разрушается.
			   
			   Однако умные указатели спасают ситуацию 
			   std::shared_ptr<BadBase> obj = std::make_shared<BadDerived>();
			   
			   A это НЕТ!
			   std::unique_ptr<BadBase> obj = std::make_unique<BadDerived>();
			   		   
 -  Why are destructors important:
		Answer: Destructors are important because they give us a chance to free up an object allocated resources before the object goes out of scope. 
				Since C++ doesnt have a garbage collector, resources that we dont free ourselves will never be released back to the system, eventually making things grind to a halt.
				
 -  Never decalre destructor without reason!
    Destructor doing nothing --> they disablee move ctor and assignment
	
 -  What is a Default Constructor?
		Answer: Default constructor is a constructor that either has no arguments or if there are any, then all of them are default arguments.
		
 -  Зачем обявлять деструктор override в базовом классе?
		Answer: Что бы при компиляции удостоверить что конструктор базового класса обявлен как virtual	

 -  Сколько в памяти занимает произвольная структура?
		Ответ: sizeof всех членов + 
			   остаток для выравнивания (по умолчанию выравнивание 4 байта) + 
			   sizeof указателя на vtable (если есть виртуальные функции) + 
			   указатели на классы предков, от которых было сделано виртуальное наследование (размер указателя * количество классов)
 
 -  Что такое чисто виртуальные функции: (Абстрактная функция)
		Ответ: Чисто виртуальная функция (pure virtual function) является функцией, которая объявляется в базовом классе, но не имеет в нем определения. 
			   Поскольку она не имеет определения, то есть тела в этом базовом классе, то всякий производный класс обязан иметь свою собственную версию определения.
			   Для объявления чисто виртуальной функции используется следующая общая форма:
				
				virtual void func() = 0;
		 
 -  Что такое абстрактный класс:
		Ответ: Если какой-либо класс имеет хотя бы одну чисто виртуальную функцию, то такой класс называется абстрактным (abstract)
		
 -  Что такое чисто абстрактный класс:
		Ответ: Класс все методы которого абстрактные


 -  Ковариантность:

    Пусть имеется класс Base, и класс Derived - его наследник. Тогда возможно определить классы A и B, с виртуальным методом foo() следующим образом:

	 struct Base { };
    struct Derived : Base { };

    struct A {
        virtual Base* foo() { return nullptr; }
    };

    struct B : A {
        Derived* foo() override { return nullptr; }
    };
    
		
 -  Что такое Aggregate initialization:
		Ответ: Initializes an aggregate from braced-init-list,
		Может применяться:
			- для array type
			- class type (typically, struct or union), that has
			  > no private or protected direct (since C++17)non-static data members
			  > no user-declared constructors
			  > no user-provided constructors (explicitly defaulted or deleted constructors are allowed)
			  > no user-provided, inherited, or explicit constructors (explicitly defaulted or deleted constructors are allowed)
			  > no user-declared or inherited constructors
			  > no virtual, private, or protected (since C++17) base classes
			  > no virtual member functions
			  > no default member initializers

 -  Что такое friend class:
		Answer: A friend class is one that is allowed access to the private and protected members of any class that has declared it a friend. 
				The property is not inherited (subclasses of friends do not become friends automatically), and not transitive (friends of friends are not friends).
  
 -  Как классу вызвать свой  же конструктор из другого конструктора:
		Ответ: class Base {
			    public:
					Base() { std::cout << "Base::Base()" << std::endl; }
					Base(CString _name) : Base() { std::cout << "Base::Base(). [Name: " << this->name << "]" << std::endl; }
				}
				
 -  Что дает модификатор PRIVATE:
 		Ответ: The class members declared as private can be accessed only by the functions inside the class.
		       They are not allowed to be accessed directly by any object or function outside the class. 
			   Can be accessed by:
			   1. functions inside the class
			   2. friend functions
			   3. friend classes
			   4. nested classes
			   5. friend derived classes
			   
  - Что дает модификатор PROTECTED:
 		Ответ: Protected access modifier is similar to that of private access modifiers, the difference is that the class 
		       member declared as Protected are inaccessible outside the class but they can be accessed by any subclass(derived class) of that class.	
			   Can be accessed by:
			   1. functions inside the class
			   2. "functions inside subclass the class"
			   3. friend functions
			   4. friend classes
			   5. nested classes
			   6. friend derived classes			   
  
  - Что дают разные модификаторы при наследовании?
 		Ответ: PUBLIC наследование: унаследованные public остаются public, 
									унаследованные члены protected остаются protected
									унаследованные члены private остаются недоступными для дочернего класса.
									Ничего не меняется.
			   PROTECTED наследование: члены public и protected становятся protected
									   члены private остаются недоступными.
			   PRIVATE наследование: члены public и protected становятся private
									 члены private остаются недоступными.
  
  - Какой модификатор наследования используется по умолчанию:
		Ответ: По умолчанию классы наследуются, как private, а структуры - как public
  
  -  Что делает ключевое слова default:
		Ответ: Это означает, что вы хотите использовать сгенерированную компилятором версию этой функции, поэтому вам не нужно указывать тело.
		       либо в switch case операциях

  -  Что делает ключевое слова delete:
		Ответ: Вы также можете использовать = delete, чтобы указать, что вы не хотите, чтобы компилятор автоматически сгенерировал эту функцию.
		
		struct NonCopyable {
			NonCopyable() = default;
			NonCopyable(const NonCopyable&) = delete;
			NonCopyable& operator=(const NonCopyable&) = delete;
		};	
		
  -  Как запретить наследование класса:
		Ответ:  Сделать его конструктор приватным
		        Сделать его конструктор = delete;
				Сделать class final;
		
  -  How can we disable inheritance of the class:
     	Ответ:  Just declare the class final
				class Base final { };

  -  Возможен ли полиморфизм при private наследоании:
       	Ответ: Нет. нельзя привести Derived сlass к base-овому.
		       A class does inherit the implementation with private inheritance.

  -  How can I set up my member function so it won’t be overridden in a derived class:
     	Ответ:  Just declare the function final
		
  -  Наследуется ли copy assignment operator:
		Ответ:  Если в порожденном класе assignment operator НЕ определен то БУДЕТ вызван assignment operator БАЗОВОГО класса
		        Если в порожденном класе assignment operator ОПРЕДЕЛЕН то assignment operator базового класса НЕ будет вызван 
			
  - Что такое виртуальный класс и какой порядок вызова их конструкторов:
    	Constructors for Virtual Base Classes:
				Constructors for virtual base classes are invoked before any non-virtual base classes. 
				If the hierarchy contains multiple virtual base classes, the virtual base class constructors are invoked in the order in which they were declared. 
				Any non-virtual bases are then constructed before the derived class constructor is called. 
				If a virtual class is derived from a non-virtual base, that non-virtual base will be first, so that the virtual base class can be properly constructed.

  - В каких случаях assignment operator будет установлен компилятором в delete в случаях если:
    	- Класс имеет не статическую переменую of reference type
    	- Класс имеет не статическую const переменую
    	- Класс имеет явно заданный move constructor
		- Класс имеет явно заданный move assignment operator

  - Вызов virtual функции в конструкторе и деструкторе класса:
    	Ответ: Не нужно так! Очень опастно!
		       Будут вызваны функции именно этого самого базового класса а не их спецификации в порождённых классах.
			   Если в Base классе эти методы не специфицированы получим Pure Virtual Exception.

  -  /** What is a conversion constructor" **/?
     	Ответ:  A constructor declared without the function-specifier explicit specifies a conversion from the types of its parameters to the type of its class. 
		        Such a constructor is called a converting constructor.
				
		class IntClass {
			int num;
		public:
			IntClass(int a) : num(a) { }
			// conversion from User-defined type to Basic type 
			operator int() const { return num;	}
		};


  -  Polymorphism types:
	  -	Function Overloading: When there are multiple functions with same name but different parameters then these functions are said to be overloaded. 
	      Functions can be overloaded by change in number of arguments or/and change in type of arguments.
	  -	Operator Overloading: C++ also provide option to overload operators. For example, we can make the operator (‘+’) for string class to concatenate two strings.
	  -	Runtime polymorphism: This type of polymorphism is achieved by Function Overriding
				
  -  'Hiding of all overloaded methods with same name in base class:'
     	Ответ:  In C++, if a derived class redefines base class member method then all the base class methods with same name become hidden in derived class.
		
		Workaroud: 
			make func() s virtual and call Derived::func() this way:     dynamic_cast<Derived*>(b1)->info("");
		
		class Base  { 
		public: 
			int func()  {  cout<<"Base::func() called";   } 
			int func(int i)  {  cout<<"Base::func(int i) called";  } 
		}; 
		  
		class Derived: public Base  { 
		public: 
			int func(const std::string& text)  { cout<<"Derived::func() called";  } 
		}; 

		int main() { 
			Derived d; 
			d.func(5);  // Compiler Error 
		} 


  -  'Local Classes in C++':
     	Ответ:  A class declared inside a function becomes local to that function and is called Local Class in C++
		
		void fun() {
			class Object { // Local class  
			public:
				Object() {
					std::cout << "Local object created Object::Object()" << std::endl;
				}
			};
			Object obj;
		}


  -  'Что такое ad-hoc polymorphism':
		Ответ:  Operator overloading polymorphism (Ad-hoc polymorphism is also known as overloading)
		
		class IntClass {
			int num;
		public:
			IntClass(int a) : num(a) { }
			operator int() const {  return num; } // conversion from User-defined type to Basic type 
		};

		void show(int x) { std::cout << x << std::endl; }
		
		void main() {
			IntClass i = 100;
			show(746); // outputs 746 
			show(i);   // outputs 100 
		}
		

  -  'Что делает ключевое слова explicit':
		Ответ:  Запрещает явное преобразование типов в конструкторах и операторах bool() к примеру:

		class Base {
		public:
			explicit Base(int i) {  // converting constructor
				std::cout << "Base::Base(" << i << ")" << std::endl;
			}
			explicit Base(int a, int b) { // converting constructor (C++11)
				std::cout << "Base::Base(" << a << ", " << b << ")" << std::endl;
			}
			explicit operator bool() const {
				std::cout << "Base::Base bool()" << std::endl;
				return true;
			}
		}
		
		void CTor_Operator_Bool_2() {
			Base b1(1);
			Base b2 = 2;             // ERROR
			Base b3{ 3, 4 };
			Base b4 = { 5, 6 };      // ERROR
			Base b5 = (Base)7;
			if (b1) {  /* OK: A::operator bool(). */ }
			bool na1 = b1;           // ERROR   
			bool na2 = static_cast<bool>(b1); // OK: static_cast performs direct-initialization
		}

  -  'How to make overloading work for derived classes':
		class B {
		public:
			void f(int i) {  std::cout << "f(int): " << i + 1 << std::endl; }
		};

		class D_Good : public B {
		public:
			using B::f; ///////// Here it is. Иначе вызов pd->f(2) напечатает "f(double): "
			void f(double d) { std::cout << "f(double): " << d + 1.3 << std::endl; }
		};

		void Overload_Test___GoodUsage(){
			D_Good* pd = new D_Good;
			pd->f(2);
			pd->f(2.3);
		}
		
  -  'Нюансы использования Virtual функций':
			class Base {
			public:
				virtual void show() { std::cout << "In Base" << std::endl; }
			};

			class Derived : public Base {
			public:
				virtual void show() { std::cout << "In Derived" << std::endl; }
			};

			void Call_BaseClass_Func_Hack() {
				Base *ptr = new Derived;
				ptr->Base::show();  // "In Base" will be printed
			}

			void Call_Method_FromObjectReference() {
				Derived d;
				Base &br = d;
				br.show();  // "In Derived" will be printed
			}
			
  -  'Using default value with virtual function'

     [function signature is the part of call]
		struct Base {
			virtual void print(int i = 10) noexcept { std::cout << "Base::print(" << i << ")\n"; }
		};

		struct Derived : public Base {
			virtual void print(int i = 14) noexcept override { std::cout << "Derived::print(" << i << ")\n"; }
		};

		void Test() {
			Base* obj = new Derived();
			obj->print();
		}
		
		Результат: Derived::print(10)!!

	- 'Как СКРЫТЬ метод базового класса в производном'

		struct A {
		    void method1();
		};

		struct B: public A {
		    void method2();

		    private:
		    using A::method1;   // <=====================
		};


========================================================================================================================
								Virutal Mem
========================================================================================================================

Virtual memory : - то что видит программа
Physical memory: - hardware RAM установленный на компьютере  [ https://www.youtube.com/watch?v=59rEMnKWoS4 ]

Размер Virtual memory может быть (и кажется всегда) больше чем Physical memory:

В большинстве современных операционных систем виртуальная память организуется с помощью страничной адресации.
Оперативная память делится на страницы: области памяти фиксированной длины (например, 4096 байт), которые являются минимальной единицей выделяемой памяти 
(то есть даже запрос на 1 байт от приложения приведёт к выделению ему страницы памяти).
Исполняемый процессором пользовательский поток обращается к памяти с помощью адреса виртуальной памяти, который делится на номер страницы и смещение внутри страницы.
Процессор преобразует номер виртуальной страницы в адрес соответствующей ей физической страницы при помощи буфера ассоциативной трансляции (TLB). 


▪ Page Tables: [ https://www.youtube.com/watch?v=KNUJhZCQZ9c ]
	
▪ Page Faults: [ https://www.youtube.com/watch?v=bShqyf-hDfg ]

▪ Зачем:
	- Недостаточно RAM
	- фрагментация памяти
	- programs writing over each other

▪ Profit:
	- поддержка изоляции процессов и защиты памяти путём создания своего собственного виртуального адресного пространства для каждого процесса
	- поддержка отображённых в память файлов, в том числе загрузочных модулей
	- поддержка разделяемой между процессами памяти, в том числе с копированием-по-записи для экономии физических страниц
	- Память используется более эффективно
	- Можно запускать проложения потребляющие больше чем реально есть физической памяти
	- Позволяет оградить приложения от самостоятельного управления разделяемой памятью

============================================================================================================================================================	
    				                           			  Кэш процессора | Cache Line
============================================================================================================================================================

кэш [Cache ](сверхоперативная память), используемый микропроцессором компьютера для уменьшения среднего времени доступа к компьютерной памяти.
Является одним из верхних уровней иерархии памяти
Кэш использует небольшую, очень быструю память (обычно типа SRAM), которая хранит копии часто используемых данных из основной памяти.
Если большая часть запросов в память будет обрабатываться кэшем, средняя задержка обращения к памяти будет приближаться к задержкам работы кэша.


Данные между кэшем и памятью передаются блоками фиксированного размера, также называемыми линиями кэша (англ. cache line) или блоками кэша.


The data in a cache is grouped into blocks called cache-lines, which are typically 64 or 128 bytes wide. 
These are the smallest units of memory that can be read from, or written to, main memory. 
This works well in most programs as data that is close in memory is often needed close in time by a particular thread. 
However, this is the root of the false sharing problem.

============================================================================================================================================================	
    				                           			  Memory                                                                    
============================================================================================================================================================
		
  Const Data: The const data area stores string literals and other data whose values are known at compile time.  No objects of class type can exist in this area. 
              All data in this area is available during the entire lifetime of the program.
              ther, all of this data is read-only, and the results of trying to modify it are undefined.
				  
  Stack:      The stack stores automatic variables. Typically allocation is much faster than for dynamic storage (heap or free store) because a 
              memory allocation involves only pointer increment rather than more complex management.  
              Objectsare constructed immediately after memory is allocated and destroyed immediately before memory is deallocated.

  Free Store  The free store is one of the two dynamic memory areas, allocated/freed by new/delete.  
              Object lifetime can be less than the time the storage is allocated; that is, free store objects can have memory allocated without
              being immediately initialized, and can be destroyed without the memory being immediately deallocated.  

  Heap        The heap is the other dynamic memory area, allocated/freed by malloc/free and their variants.
              Note that while the default global new and delete might be implemented in terms of malloc and free by a particular compiler, the heap is not the same
              as free store and memory allocated in one area cannot be safely deallocated in the other. 
              Memory allocated from the heap can be used for objects of class type by placement-new construction and explicit destruction.  
              If so used, the notes about free store object lifetime apply similarly here.

  Global:     Global or static variables and objects have their storage allocated at program startup, but may not be initialized until after the programhas
  Static	  begun executing.  For instance, a static variable in a function is initialized only the first time program execution passes through its definition.
              The order of initialization of global variables across translation units is not defined, and special care is needed to manage dependencies between global objects (including class statics). 

  - Как указать что оператор new() не должен выбрасывать исключения: [std::nothrow]
		By default, when the new operator is used to attempt to allocate memory and the handling function is unable to do so, a bad_alloc exception is thrown. 
		But when nothrow is used as argument for new, it returns a null pointer instead.

		// indicate that these functions shall not throw an exception
		char* p = new (std::nothrow) char [1048576]; 
		
  - В чем различия между delete и delete[]?
 		Ответ: delete предназначен для уничтожения объектов, память под которые выделена при помощи new(). 
		       delete[] для объектов выделенных при помощи оператора new[]().
			   
		       Foo *pFoo = new Foo();
		       delete pFoo;
		       Foo *pFooArray = new Foo[10]();
		       delete[] pFoo;
			
  -  Write simple program to overload new and delete operator in C++:
     		Ответ: 
				class CustomMemory{
				public: 
					void* operator new(size_t objectSize) {
						cout<<"Custom memory allocation"<<endl;
						return malloc(objectSize); //May Write costume memory allocation algorithm here
					} 
					void operator delete(void* ptr) {
						cout<<"Custom memory de- alloc ation"<<endl;
						free(ptr);
					}
				};
				int main(){
					CustomMemory *obj = new CustomMemory(); // call overloaded new from the class
					delete obj;                             // call overloaded delete 
				}

  - Как запретить создавать объект в куче: ( operator new() = delete;)
		class NoHeapObject {
			int id;
			std::string name;
		public:
			NoHeapObject(int userId, const std::string& userName) : id(userId), name(userName) { std::cout << __FUNCTION__ << std::endl; }
			~NoHeapObject() { std::cout << __FUNCTION__ << std::endl; }
			// Delete the new function to prevent object creation on heap
			void * operator new (size_t) = delete;
		};					
					
  - Как создать объект без конструктора по умолчанию и не задавая параметров:
  - Placement new, или как создать объект в выделенной памяти
     	Ответ:  (((placement new ))))
		
			void *raw_mem_block = operator new[](max_size * sizeof(T));
			T *pool = static_cast<T*>(this->raw_mem_block)
			for (int pos = 0; pos < 10; ++pos)
				new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
			
			вызов деструкторов
			
			// destruct the objects in objects_array in the inverse order in which they were constructed // OLD STYLE
			for (int i = 9; i >= 0; --i)
				pool[i].~T();
			
			// destruct the objects in objects_array in the inverse order in which they were constructed // NEW STYLE
			auto ptr = std::launder(reinterpret_cast<T*>(pool));
			std::destroy(ptr, ptr + max_size);
			
			operator delete[](this->raw_mem_block);  // deallocate the raw memory
			
  - ПРИМЕР 2:
		constexpr size_t size = 5;
		using T = Integer;

		void *raw_memory = operator new[](size * sizeof(T));
		T *pool = static_cast<T*>(raw_memory);
		FOR (pos, size) new (&pool[pos]) T(pos); // Create 5 Integer()'s 

		auto ptr = std::launder(reinterpret_cast<T*>(pool));
		std::destroy(ptr, ptr + size);
		operator delete[](raw_memory);
			
  - What is placement new in C++:
     	Ответ: The object is now created using the already existing memory and the lifetime starts.
			char * some_buffer = get_some_buffer();
			auto s = new (some_buffer) some_class();
			
  - Можно ли создать объект вызовом New() без выделения памяти а использовать уже выделенную:
     	Ответ:  There are times when you really want to call a constructor directly.
			    Invokinga constructor on an existing object makes no sense, because constructors initialize objects, 
				and an object can only be initialized — given its first value — once. 
				But occasionally you have some raw memorythat’s already been allocated, and you need to construct an object in the memory you have. 
				A special version of operator new called placement new allows you to do it. 
				As an example of how placement new might be used, consider this:
				This function returns a pointer to a Widget object that’s constructed within the buffer passed to the function.
				
				class Widget {
				public:
					Widget(int widgetSize);
				};
				Widget * constructWidgetInBuffer(void *buffer, int widgetSize) {
					return new (buffer) Widget(widgetSize);
				}

  - Differences between new operator and malloc() function in C++:
		new is an operator whereas malloc() is a library function.
		new allocates memory and calls constructor for object initialization. 
		But malloc() allocates memory and does not call constructor.
		Return type of new is exact data type while malloc() returns void*.
		new is faster than malloc() because an operator is always faster than a function.

  - Какие требования должны выполняться, чтобы можно было заменить new на malloc для класса?
		- надо чтобы конструктор был тривальный
		- нельзя иметь виртуального наследования и виртуальных функций
		- нельзя иметь нетрививальные конструкторы у предков и у мемберов
		- необходимо еще заменить delete на free
		
  - Explain static memory allocation 
    	Memory is allocated for the declared variable by the compiler. 
    	The address can be obtained by using ‘address of’ operator and can be assigned to a pointer. The memory is allocated during compile time. 
    	Since most of the declared variables have static memory, this kind of assigning the address of a variable to a pointer is known as static memory allocation.

  -  Explain dynamic memory allocation 
    	Allocation of memory at the time of execution (run time) is known as dynamic memory allocation. T
    	The functions calloc() and malloc() support allocating of dynamic memory. 
    	Dynamic allocation of memory space is done by using these functions when value is returned by functions and assigned to pointer variables.
		
		
============================================================================================================================================================	
    				                           			  Lamdas                                                                    
============================================================================================================================================================	
		
  - Как сделать лямбду членом класса:

		struct MyStruct {
			decltype([]() { std::cout << "I'am class lambda member\n"; }) func;
		};
	
	
  - Какие существует 'capture mode' у лямбда функций:
     	1. [&] - доступны все переменные по ссылке
		2. [&var, &var2] - доступны только переменные var и var2 по ссылке
     	3. [=] - доступны все переменные по значению
		4. [var, var2]   - доступны только переменные var и var2 по значению
		5. [integer = std::move(integer)]  - init capture makes для Move only переменных
		6. [&, this]
		

============================================================================================================================================================	
    				                           			  функциональное программирование                                                                    
============================================================================================================================================================	

Функциональное программирование — это парадигма программирования, в которой основными строительными блоками являются функции. 
В функциональном программировании функции рассматриваются как математические функции — они принимают входные данные и возвращают результат.

Основные принципы функционального программирования:

— Чистота функций: функции не должны иметь побочных эффектов и зависеть только от своих аргументов.

— Иммутабельность: данные в функциональном программировании неизменяемы.

— Рекурсия вместо циклов: в функциональном программировании циклы заменены на рекурсивные вызовы функций.

— Высшего порядка функции: функции могут принимать на вход и возвращать другие функции.

— Ленивые вычисления: вычисления откладываются до тех пор, пока результат не потребуется.

============================================================================================================================================================	
    				                           			  Вопросы                                                                    
============================================================================================================================================================	

  - Что такое Intrinsic:

	Интринсики — это функции, вызовы которых заменяются компилятором на некую внутрикомпиляторную магию.
	То есть вы думаете, что вызываете функцию, но на самом деле её не существует, и вместо неё напрямую подставляется некий машинный код вне зависимости от флагов оптимизации.

	то обёртка над функциями, которая встроена в компилятор, в таком же виде, как и обычные функции. 
	Иными словами можно сказать, что это механизм включения машинных инструкций в код.

  - strict aliasing rule:

  		Accessing a value using a type which is different from the original one is not allowed and it is classified as undefined behavior

  		In C and C++ aliasing has to do with what expression types we are allowed to access stored values through.
  		In both C and C++ the standard specifies which expression types are allowed to alias which types.
  		The compiler and optimizer are allowed to assume we follow the aliasing rules strictly, hence the term strict aliasing rule.
  		If we attempt to access a value using a type not allowed it is classified as undefined behavior(UB).
  		Once we have undefined behavior all bets are off, the results of our program are no longer reliable.
  		

  - What is Pointer Dereferencing:

  		Pointer dereferencing (*ptr) means obtaining the value stored in at the location refereed to the pointer

  - Pointer Arithmetic:

  		ptr[i] is equal to *(ptr + i)

  		address(ptr + i) = address(ptr) + (sizeof(T) * i)

  - Для каких целей применяется ключевое слово const?
		Ответ:  Позволяет задать константность объекта
				  Позволяет задать константность указателя
				  Позволяет указать, что данный метод не модифицирует члены класса, т.е. сохраняет состояние
				
  - Explain "const" reference arguments in function?
		- It protects you against programming errors that can alter data.
		- It allows function to process both const and non-const actual arguments.
		- A function without const in the prototype can only accept non constant arguments.
		- Using a const reference allows the function to generate and use a temporary variable appropriately.	

  - what is a mutator method:	
		- In computer science, a mutator method is a method used to control changes to a variable.
		  They are also widely known as setter methods. 
		  Often a setter is accompanied by a getter (also known as an accessor), which returns the value of the private member variable.
		  
  - Для чего используется ключевое слово mutable?
		- The mutable keyword can be used for class member variables.
		  Mutable variables are allowed to change from within const member functions of the class.

  - Что такое lvalue:
 		Ответ: lvalue (locator value) представляет собой объект, который занимает идентифицируемое место в памяти (например, имеет адрес).

  - Что такое rvalue:
		- rvalue определено путём исключения, говоря, что любое выражение является либо lvalue, либо rvalue. 
		  Таким образом из определения lvalue следует, что rvalue — это выражение, которое не представляет собой объект, который занимает идентифицируемое место в памяти.

  - Что такое iterator:
		- An iterator resembles a smart pointer, in the sense that it points to a particular object in a container. 
		  But iterators have additional operations besides deferencing, depending on their type: 
		  forward iterators can be incremented, bidirectional iterators can additionally be decremented, 
		  and random access iterators can additionally be moved by an arbitrary offset.

  - Что стоит учитывать при использовании auto_ptr?
		- Так как данный умный указатель реализует подход разрушающего копирования, то при присвоении его другому умному указателю оригинальный потеряет свое значение. 
		  А так же его нельзя использовать в стандартных STL контейнерах.
   
  - Для чего используется ключевое слово volatile?
     		Ответ: Для указания компилятору, что доступ к переменной может осуществляться из мест, неподконтрольных ему. 
				   А как следствие, что работу с данной переменной не нужно подвергать разного рода оптимизациям.		
				   
     		Ответ: The volatile keyword informs the compiler that a variable may change without the compiler knowing it. 
				   Variables that are declared as volatile will not be cached by the compiler, and will thus always be read from memory.

  - volatile (Wrong Uses of volatile)
		- Is not a synonym for either _Atomic (in C) or std::atomic<T> (in C++).
		- Does not use memory barriers.
		- Therefore, does not guarantee thread-safety.
		- Limits only what optimizations the compiler may do.
		- Does not limit what the hardware can do.

  - Что такое динамическая и статическая типизация?
     		Ответ: Статически типизированные языки – это языки, в которых проверка типа совершается во время компиляции, а в динамически типизированных – в рантайме. 
					Поскольку C++ является статически типизированным языком, пользователь должен сообщить компилятору, с каким типом объекта он работает во время компиляции.
					
  - Что такое абстракция? Чем она отличается от инкапсуляции?
     		Ответ: Абстракция – это механизм предоставления только интерфейсов, сокрытия сведений о реализации и “показ” необходимых деталей функционала. 
				   Инкапсуляцию можно понимать как сокрытие свойств и методов от внешнего мира. Класс является лучшим примером инкапсуляции в C++.
				   
  - Виртуальное наследование?
   		Ответ: Виртуальное наследование (virtual inheritance) предотвращает появление множественных объектов базового класса в иерархии наследования. 
				   
  -  Что такое класс хранения (What is a storage class?)?		
   		The storage class specifiers are a part of the decl-specifier-seq of a name's declaration syntax. 
		Together with the scope of the name, they control two independent properties of the name: its storage duration and its linkage.  
   		Storage class specifies the life or scope of symbols such as variable or functions:  
		- auto
		- static
		- register
		- extern
		- mutable.
		- thread_local
		
  -  What is 'thread_local' storage class?:
   		thread_local -> thread storage duration.
		thread_local -> may be combined with static or with extern
		
		The thread_local keyword is only allowed for objects declared at namespace scope, objects declared at block scope, and static data members.
		It indicates that the object has thread storage duration. 
		It can be combined with static or extern to specify internal or external linkage (except for static data members which always have external linkage),
		respectively, but that additional static doesn't affect the storage duration.
		
  -  Может ли lamba функция быть constexpr:
   		Since C++17, lambdas are implicitly constexpr if possible.
		
		auto squared3 = [](auto val) constexpr { // OK since C++17
			return val*val;
		};
		auto squared3i = [](int val) constexpr -> int { // OK since C++17
			return val*val;
		};
		
		// Constexpr sort example:
		
		auto sort_constexpr = [](auto data) constexpr -> decltype(data) {
			std::sort(std::begin(data), std::end(data));
			return data;
		};
		constexpr auto sorted_array = sort_constexpr(std::array<int, 5>{5, 4, 3, 2, 1});
		
  -  Как рекурсивно вызвать lamba функцию: [auto здесь не сработает только 'std::function<int(int const)>' для типа функции]
		std::function<int(int const)> lfib = [&lfib](int const n) {
			if (1 == n) return 0;
			else if (2 == n) eturn 1;
			else return lfib(n - 1) + lfib(n - 2);
		};
		
		Lambda closure cannot remain unspecified, that is, be declared with the auto specifier. 
		It is not possible for a variable declared with the auto type specifier to appear in its own initializer because the type of the 
		variable is not known when the initializeris being processed. 
		Therefore, you must specify the type of the lambda closure. 
		The way we can do this is using the general purpose function wrapper std::function.

  -  What will i and j equal after the code below is executed? Explain your answer.
		  int i = 5;
		  int j = i++;
		  std::cout << "i = " << i << ", j = " << j << std::endl;   // [ Ответ: i = 6, j = 5 ]
			
  -  Assuming buf is a valid pointer, what is the problem in the code below? What would be an alternate way of implementing this that would avoid the problem?
			size_t sz = buf->size();
			while ( --sz >= 0 ) {
			   /* do something */
			}
			
			The problem in the above code is that --sz >= 0 will always be true so you’ll never exit the while loop (so you’ll probably end up corrupting memory or causing some sort of memory violation or having some other program failure, depending on what you’re doing inside the loop).
			The reasons that --sz >= 0 will always be true is that the type of sz is size_t. size_t is really just an alias to one of the fundamental unsigned integer types. Therefore, since sz is unsigned, it can never be less than zero (so the condition can never be true).

  -  Is it possible to have a recursive inline function?
     		Ответ: Although you can call an inline function from within itself, the compiler may not generate 
				   inline code since the compiler cannot determine the depth of recursion at compile time. 
				   A compiler with a good optimizer can inline recursive calls till some depth fixed at compile-time (say three or five recursive calls), 
				   and insert non-recursive calls at compile time for cases when the actual depth gets exceeded at run time.

  -  In what situations initialization list is must and assignment in constructor body does not help?

       		Ответ 1: initialization of constant & reference data member of a class.
					class Diamond{
						const int a;//constant variable
						int &b; //reference	
					public:
						//initialize const and reference variable in initialization list.
						Diamond( int i, int j, int k):a(i),b(j) //initialization list. {
							//Construcotor body	
							//compiler flashes error on initializing const and reference variable here.
							a=5;// error
							b=5;// error
						} 
					};
					
       		Ответ 2: Initialization of base class data members from derive class
			
  -  При использовании 'initialization list' в каком порядке инициализируются пепременные класса
     		Ответ: В порядке их определения в классе. Например:
			class Var {
				int a;
				int b;
				int c;
			public:
				Var(): c(++i), b(++i), a(++i) {}
			};
			Результат: a = 1, b =2 ,c =3; !!!!!!!!!!!!!!!!!!!!!
		
  -  How GLOBAL variable initialyzed:
	 	Если глобальная переменная не инициализирована при определении то она распологается в BSS сегменте памяти и инициализируется равной 0.
  
  -  What is early binding:
	 	In early binding, the compiler matches the function call with the correct function definition at compile time. 
	 	It is also known as Static Binding or Compile-time Binding
		
  -  What is late binding:
 	 	In the case of late binding, the compiler matches the function call with the correct function definition at runtime.
		It is also known as Dynamic Binding or Runtime Binding
		
  -  What do you mean by internal linking and external linking in c++?
	 	A symbol is said to be linked internally when it can be accessed only from with-in the scope of a single translation unit. 
	 	By external linking a symbol can be accessed from other translation units as well. This linkage can be controlled by using static and extern keywords.

  -  What do you mean by translation unit in c++?
     	Ответ: This effective content is called a translation unit. In other words, a translation unit consists of
			   Contents of source file
			   Plus contents of files included directly or indirectly
			   Minus source code lines ignored by any conditional pre processing directives ( the lines ignored by #ifdef,#ifndef etc)

  -  Explain dangling pointer:
     	Ответ: When the address of an object is used after its lifetime is over, dangling pointer comes into existence. 
				Some examples of such situations are: Returning the addresses of the automatic variables from a function or using the address of the memory block after it is freed
				
  -  How do you differentiate between aggregation and association
     	Ответ:  Association: Foo has a pointer to Bar object as a data member, without managing the Bar object -> Foo knows about Bar
				Composition: Foo has a Bar object as data member -> Foo contains a Bar. It cant exist without it.
				Aggregation: Foo has a pointer to Bar object and manages the lifetime of that object -> Foo contains a Bar, but can also exist without it.

  -  What is a SFINAE:
       	Ответ:  Substitution Failure Is Not An Error
				Правило SFINAE гласит: Если не получается рассчитать окончательные типы аргументов (провести подстановку шаблонных параметров) перегруженной шаблонной функции, компилятор не выбрасывает ошибку, а ищет другую подходящую перегрузку. Ошибка будет в трёх случаях:
				 - Не нашлось ни одной подходящей перегрузки.
				 - Нашлось несколько таких перегрузок, и Си++ не может решить, какую взять.
				 - Перегрузка нашлась, она оказалась шаблонной, и при инстанцировании шаблона случилась ошибка.
				
  -  What is a One Definition Rule
       	Ответ:  The One Definition Rule (ODR) is an important rule of the C++ programming language that prescribes 
				that objects and non-inline functions cannot have more than one definition in the entire program and template 
				and types cannot have more than one definition by translation unit. It is defined in the ISO C++ Standard
				!!!!  non-inline  only !!!!
		
  - Какие проблемы может вызвать то что One Definition Rule не применяется для inline function:
       	Ответ:  functions with internal linkage may be duplicated within a program (i.e., the object code for the program may
			    contain more than one copy of each function with internal linkage), and this duplication includes static objects contained 
				within the functions. The result? If you create an inline non-member function containing a local static object, you may end up with more
				than one copy of the static object in your program! 
				!!! So don’t create inline non-member functions that contain local static data.†

  - Explain One-Definition Rule (ODR):
       	According to one-definition rule, C++ constructs must be identically defined in every compilation unit they are used in.
		As per ODR, two definitions contained in different source files are called to be identically defined if they token-for-token identical. 
		The tokens should have same meaning in both source files.
		Identically defined doesn’t mean character-by-character equivalence. Two definitions can have different whitespace or comments and yet be identical.
		
  -  What is a DRY:
       	Ответ:  Dont repeat yourself (DRY, or sometimes do not repeat yourself) is a principle of software development aimed at 
				reducing repetition of software patterns, 	replacing it with abstractions or using data normalization to avoid redundancy
				
  -  What is a SOLID:
		S: 'Single-responsibility principle'

		   A class should only have a single responsibility, that is, only changes to one part of the software-s specification should be able to affect the specification of the class.
			
		O: 'Open–closed principle'

		   Software should be open for extension, but closed for modification - we should be able to add new features to our software without having to change existing code.
		   This simply means that a class should be easily extendable without modifying the class itself

		   открыты для расширения: означает, что поведение сущности может быть расширено путём создания новых типов сущностей.
		   закрыты для изменения: в результате расширения поведения сущности, не должны вноситься изменения в код, который эту сущность использует.
		   
		L: 'Liskov substitution principle'

			Objects in a program should be replaceable with instances of their subtypes without altering the correctness of that program.
			See also design by contract.
			
		I: 'Interface segregation principle'

			Many client-specific interfaces are better than one general-purpose interface
			Client classes should not be forced to depend on interfaces that they don-t use.
			This means that we should create specialised interfaces for each client, so that they only have to depend on the methods that they need.
			
		D: 'Dependency inversion principle'

		   One should depend upon abstractions, [not] concretions.
		   Entities must depend on abstractions not on concretions. 

		   High-level modules should not depend on low-level modules --> both should depend on abstractions.
		   This means that we should decouple our code so that the high-level modules don-t have to know about the details of the low-level modules.
			
  - Increment vs decrement operators:
       	Ответ:  Pre-increment and pre-decrement operators increments or decrements the value of the object and returns a reference to the result.
				Post-increment and post-decrement creates a copy of the object, increments or decrements the value of the 
				object and returns the copy from before the increment or decrement.	
 
  - Do you think the following code is fine? If not, what is the problem?
						T *p = 0;
						delete p;
      	Ответ:  The program will crash in an attempt to delete a null pointer.			

  - Explain Stack unwinding.
     	Ответ:  Stack unwinding is a process during exception handling when the destructor is called for all 
		        local objects between the place where the exception was thrown and where it is caught.

  - How to declare function to indicate that it emits no Exception:
    	Ответ:  virtual void Test() const noexcept { }
		
  - How to declare function to indicate that it DOES throw exception:
    	Ответ:  virtual void Test() const noexcept(false)  { }

  - Explain deep copy and a shallow copy.
     	Deep copy : It involves using the contents of one object to create another instance of the same class. 
					Here, the two objects may contain the same information but the target object will have its own buffers and resources. 
					The destruction of either object will not affect the remaining objects.

     	Shallow copy : It involves copying the contents of one object into another instance of the same class. 
					   This creates a mirror image. The two objects share the same externally contained contents of the other object to be unpredictable.
					   This happens because of the straight copying of references and pointers.				   
					   
  - Heap vs Stack: 
    	Stack: Dramatically faster to allocate. It is done in O(1) since it is allocated when setting up the stack frame so it is essentially free. 
			   The drawback is that if you run out of stack space you are boned. You can adjust the stack size, but IIRC you have ~2MB to play with. 
			   Also, as soon as you exit the function everything on the stack is cleared. 
			   So it can be problematic to refer to it later. (Pointers to stack allocated objects leads to bugs.)
			   
    	Heap:  Dramatically slower to allocate. But you have GB to play with, and point to.		   

  - Можно ли из main() вызвать main():
    	Answer:  Да.    int main() {  
							static int N = 10; 
							if (N-- > 0) { 
								printf("%d ", N); 
								main(); 
							} 
						} 
  - How to call some function before main() function in C++:
		  class BeforeMain {
			public:
				BeforeMain() { func(); }
				void func() { std::cout << "Inside the other function" << std::endl; }
			};

			BeforeMain obj;

			int main() {
				std::cout << "Hello world" << std::endl;
			}

  - Difference between std::quick_exit and std::abort:
     	Answer: std::quick_exit() causes normal program termination to occur without completely cleaning the resources
				at_quick_exit( void (*func)(void) )takes a functions as parameter which is to be executed when the quick_exit() function is called. 
													The functions registered to this are called on the reverse order of their execution.
				std::abort() causes abnormal program termination unless SIGABRT is being caught by a signal handler passed to std::signal and the handler does not return.

  - How to use decltype:
		decltype is a keyword used to query the type of an expression. 
		Introduced in C++11, its primary intended use is in generic programming, where it is often difficult, 
		or even impossible, to express types that depend on template parameters.
		
		Суть этого оператора проста: decltype определяет тип выражения, которые передается ему в качестве аргумента.
		Результатом выражения decltype является полученный тип:
		                                                        std::string text = "";
		template <class T, class U>                     |       std::cout << "is_same<\"Text\", std::string> = " << std::is_same<decltype(text), std::string>() << std::endl;
		auto Mix(T Lhs, U Rhs) -> decltype(Lhs + Rhs) { |       int& foo(int& i) { return i; }
			return Lhs + Rhs;                           |       float& foo(float& f) { return f; }
		}                                               |       template <class T> auto transparent_forwarder(T& t) -> decltype(foo(t)) { 
													    |			return foo(t); 
														|       }
		********************
		
		int some_int;
		decltype(some_int) other_integer_variable = 5;
 
  - Будет ли член класса Base  vector будет доступен из метода TEST(). (НЕТ. нужна деректива using Base<Type>::vector;)

		template<typename Type>
		class Base {
		protected:
			std::vector<Type> vector;
		};

		template<typename Type>
		class Derived : public Base<Type> {
		private:
			// using Base<Type>::vector;

		public:
			void TEST() noexcept {
				vector.reserve(11);
			}
		};

  - How to Delete specific template specialisation:
		template <typename T>
		class ComplexNumber {
			T x;
			T y;
			public:
				ComplexNumber(T a, T b) : x(a), y(b){ }
				void display() { std::cout << x << " + i" << y << std::endl; }
				// Deleted template specialisation 
				ComplexNumber(char a, char b) = delete;
				// Deleted template specialisation  
				ComplexNumber(double a, double b) = delete;
		};

		so following code will produce errors:
		
			ComplexNumber<double> obj2(1.0,2.0);
			ComplexNumber<char> obj3('1' , '2');

  - What is the static variable:
    	Answer:  Static variables are stored in static memory, sometimes referred to as the heap, rather than on the stack. 
				 It is important to know that these variables are only initialized once, and the scope persists throughout the entire execution.
				 A static variable inside a function will keep track of its value after the function ends, and remains unchanged with subsequent calls to the same function. 
				 It is similar to a global variable, but with the accessibility limited to the function.

  - The main characteristics of static functions are as follows:
     	Ответ:  It is without the a “this” pointer. This is because it has no object address and hence “this” pointer is not passed as an internal parameter to the same. 
				Also it can can be invoked even before creation of objects using scope resolution operator.
				It cant directly access the non-static members of its class.
				It cant be declared const, volatile or virtual.
				It doesnt need to be invoked through an object of its class, although for convenience, it may.		

  - Will the inline function be compiled as the inline function always? Justify your answer.
    	Ответ: An inline function is a request and not a command. 
			   Hence it wont be compiled as an inline function always.
			   Inline-expansion could fail if the inline function contains loops, the address of an inline function is used, or an inline function is called in a complex expression. 
			   The rules for in-lining are compiler dependent.

  - What are the advantages and disadvantages of using inline and const ?
    	Advantages:    It does not require function calling overhead
				       It additionally save overhead of variables push/pop on the stack, while function calling.
					   It also save overhead of return call from a function.
					   It increases locality of reference by utilizing instruction cache.
					   Inline function may be useful (if it is small) for embedded systems because inline can yield less code than the function call outline and return.
					   
    	Disadvantages: If you use too many inline functions then the size of the binary executable file will be large, because of the duplication of same code.
					   Too much inlining can also reduce your instruction cache hit rate, 
					   thus reducing the speed of instruction fetch from that of cache memory to that of primary memory.
					   It may cause compilation overhead as if somebody changes code inside inline function than all calling location will also be compiled.

  - While overloading a binary operator can we provide default values? Justify your answer.
    	Ответ: No!. This is because even if we provide the default arguments to the parameters of the overloaded 
		        operator function we would end up using the binary operator incorrectly. 	  This is explained in the following example:
                sample operator+ (sample a, sample b = sample (2, 3.5f )) { /* Do something */ }
                void main() {
					sample s1, s2, s3 ;
					s3 = s1 + ; 
                }
				
  - What is the “Named Constructor Idiom”:
	  class Point {
		public:
		   static Point rectangular(float x, float y);      // Rectangular coords
		   static Point polar(float radius, float angle);   // Polar coordinates

		private:
		   Point(float x, float y);     // Rectangular coordinates
		   float x_, y_;
		};
		inline Point::Point(float x, float y): x_(x), y_(y) { }
		inline Point Point::rectangular(float x, float y) { return Point(x, y); }
		inline Point Point::polar(float radius, float angle) { return Point(radius*std::cos(angle), radius*std::sin(angle)); }

		int main() {
		   Point p1 = Point::rectangular(5.7, 1.2);   // Obviously rectangular
		   Point p2 = Point::polar(5.7, 1.2);         // Obviously polar
		}

  - Прикольный Singleton:
		class Printer {
		public:
			void doSomeJob();
 
			Printer(const Printer& rhs) = delete;
		};

		Printer& thePrinter() {
			static Printer p; // the single printer object
			return p;
		}
		
  - Что такое Variable Life Time Extenstion: ['Продление жизни переменнеой. Переменная str будет доступна в методе LifeTimeExtenstionTest ']
		std::string get_temp_str() { return std::string("12345"); }
		
		void LifeTimeExtenstionTest() {
			const std::string& str = get_temp_str();
		}

  - Если есть класс, на который нет внешних указателей и ссылок, то можно ли его перемещать через memcpy?
		Ответ - во-первых, разумеется, не должно быть мемберов с указателями внутрь класса. Во-вторых, если есть виртуальное наследование, то это становится implemenation dependent, некоторые компиляторы хранят указатель на виртуальный базовый класс не сдвигом, а голым указателем. MSVC и IntelC - указателем на внешнюю таблицу, gcc - пойнтером, больше не знаю.

  - Can I initialize my static member data in my constructor’s initialization list
    	Ответ: No!. you must explicitly define your class’s static data members.
		
  - Can I initialize my static member data in within class declaration:
    	yes. using INLINE
		class MyClass {
			static inline std::string name = ""; // OK since C++17
		};


============================================================================================================================================================	
								                    Copy-and-swap:
============================================================================================================================================================				

🔹Create-Temporary-and-Swap

	struct String {
	    char* str; 

	    String& operator=(const String & s) {
	        String temp(s);    // Copy-constructor -- RAII
	        temp.swap(*this);  // Non-throwing swap
	        return *this;
	    }

	    /** Old resources released when destructor of temp is called. **/
	    void swap(String & s) noexcept { /** Also see the non-throwing swap idiom: **/
	        std::swap(this->str, s.str);
	    }
	};

					
============================================================================================================================================================	
								                    Compiller dicretives:
============================================================================================================================================================				

  ▪  -fsanitize-address-use-after-scope
  ▪  gcc address sanitizer (-fsanitize=address)	
				
============================================================================================================================================================	
								                    Tricky Questings
============================================================================================================================================================		
				
▪ What is the output of the following program?: [Fine, Nine,Compile error, Runtime error].  Answer: Nine

		char s[] = "Fine";
		*s = 'N';
		std::cout << s << std::endl;

▪ What is the output of the following program?: 

		char s1[] = "C++";
		std::cout << s1 << " ";
		s1++;                   '  <---- Compile error   '
		std::cout << s1 << " ";

▪ Const propagation for pointers !!!! ----  'Compiles despite function being declared const' ----

		class Foo {
		public:
		    Foo(int* ptr) : ptr_{ptr} { }

		    auto set_ptr_val(int v) const {
		        *ptr_ = v; // Compiles despite function being declared const!
		    }

		private:
		    int* ptr_{};
		};

▪ Как объвить метод класса что можно вызвать только для R_Value references

		struct FooBar {
		    auto func() && {}
		};


	   auto a = FooBar{};
	   a.func();             // Does not compile, 'a' is not an r-value
	   std::move(a).func();  // Compiles
	   FooBar{}.func();      // Compiles


▪ What is the output of the following program: [0 , 65535 or 32767].  Answer: 0

		short unsigned int i = 0;
		std::cout << i-- << std::endl;
			
▪ In the following program f() is overloaded ?  Answer:  No, as both the functions signature is same.

		void f(int x) { }
		void f(signed x) {}
			
▪ What is the output of the following program?.  Answer: Compile error

		int r, x = 2; 
		float y = 5;
		r = y%x;       '<---- Compile error' | 'error: invalid operands of types ‘float’ and ‘int’ to binary ‘operator%’'
		cout<<r; 
		
▪ Checking if the number is even or odd without using the % operator:
	
		if (num & 1) cout << "ODD"; 
		else cout << "EVEN"; 
				
▪ Fast Multiplication or Division by 2

		n = n << 1;   // Multiply n with 2 
		n = n >> 1;   // Divide n by 2 
		
▪ ---------------------- What the output? Will this compile??? ----------------------

		struct S {
			S() { std::cout << "S::S()" << std::endl;}
			~S() { std::cout << "S::~S()" << std::endl; }
		};
		S get_S() { return {}; }

		void Extend_LocalObject_Lifetime() {
			const auto& sObjRef = get_S(); // Conts helps to extend S local ref life tine
			// auto& sObjRef = get_S(); // THIS IS WILL NOT EVEN COMPILE
			std::cout << "OK!!" << std::endl;
		}		
		
		Ответ: S::S() '\n' OK!!'\n' S::~S()'\n'
		
▪ What will happen: [Out put  = 0]
		
		std::map<std::string, int> dict;
		dict["hey"] = 123;
		std::cout << dict["hye"] << std::endl;
		
▪ What wrong with this code: [won’t even complie. operator[] modify the Map]
	
		void Widget::config(const std::map<std::string, std::string>& settings) const {
			mTimeout = settings["timeout"];
			mSize = settings["max_size"];
		}
		
▪ What wrong with this code: [лок не сработает - как как переменная std::unique_lock<std::mutex> не именована]
	
		void Foo() noexcept {
			std::unique_lock<std::mutex>(m_mutex);
			do_somethig();
		}
		
============================================================================================================================================================	
								                    Effective and Performance Modern C++:
============================================================================================================================================================
		
▪ 'Правильный пример использования emplace_back: '
		std::vector<std::string> text;
		auto& line = text.emplace_back("Hello"); //!!!!!!!!!!!!!!!!! Very cool
		line += " __ ";
		line += "World!";
		std::cout << text.back() << std::endl;
		
▪ 'Эффектривный способ передачи данных при констуировании объекта:'
		class Foo {                                                     	 				class Foo {
			int i;                                                       	   					int i;
			std::string s;                                                	   					std::string s;
		public:                             			 			<<< вместо >>>  			public:
			template<typename STR>                                         	  					Foo(int i, const std::string& str) : i(i), s(str) {
			Foo(int i, STR&& s) : i(i), s(std::forward<STR>(s)) {	        	 						}
			}                                                         	  					};
		};

▪ 'Эффектривный способ передачи данных при констуировании объекта:'
		class bar {                                                       class bar {
			std::vector<Foo> foos;                                        	  vector<Foo> foos;
		public:		                                                      public:
			template<typename... T>                ВМЕСТ                      void add(int i, double d, char c, const string& s) {
			void add(T&&... t){                                                	   foos.emplace_back(i, d, c, s); 
				foos.emplace_back(forward<T>(t)...);                          }
		    }                                                	          };
		};
		
▪ 'Передача shared_ptr<T> в метод по значению сильно медленнее чем по ссылке (примерно раз в 10):'
	void CallByRef(std::shared_ptr<Integer>& int_var) {....}    <--- быстрее чем --->     void CallByValue(std::shared_ptr<Integer> int_var) {....}
	
▪ Использовать const локальные переменные если это возможной

   Вроде как на 40% быстрее должно быть - но по моим тестам не так сильно
	
▪ 'Для проверки четности лучше использовать & место %'

   x % 2 == 0         -->        x & 1 == False
   
▪ 'Эффектривность MOVE при работе со строками: [Вариант с MOVE быстрее в два раза]'

		std::string s = std::string("a not very short string") + "b";                 std::string s = std::string("a not very short string") + "b";
		Object o(std::move(s));                                         <ВМЕСТО>      Object o(s);

▪ 'Always prefer std::array<T, Size> if its possible.'
   [deassembly code is a lot better]

▪ 'Prefer return unique_ptr<> from factories'
   shared_ptr<> is big - don-t make more than you have to

▪ 'Динамическое создание локальной объекта можно делать с помощью unique_ptr<>'
   По скорости так же как и new()
   А лучше вообще создавать на стеке

▪ 'Prefer lambdas to std::bind.'
    bind-s alot slower than Lambdas

▪ 'Invoking an std::function requires a few more operations than a lambda'
    Lambdas are FASTER !!!!!

▪ 'В циклых FOR лучше не создавить переменные вне цикла. Первый быстрее чем второй:'

                                                                       std::string val; /*** default construct **/
	 for (int i = 0; i < count; ++i) {                                 for (int i = 0; i < count; ++i) {
		std::string val = get_value(); /***  direct-init/RVO ***/         val = get_value(); /***  copy/move assignment ***/
	 }                                                                 }

▪ 'В циклых FOR. Ecли объекты более не нужны использовать нужно RValue'

		auto data = getData();
		for (auto&& enrry: data) {
			// Do something
		}

▪ 'Effective way to read from file'. 

		std::vector<std::string> lines;
		while (std::getline(file, lines.emplace_back())) { /** **/ };


▪  'C-style file management'

    std::unique_ptr<FILE, decltype(&fclose)> file(fopen("foo.txt", "r"), fclose);  // Works on malloc/free too, pass a deleter argument

	std::shared_ptr<Foo> bar(function_calling_malloc(), free);


▪ 'Список инициализации конструктора'

    Инициализируйте переменные в списке инициализации конструктора. 
	В противном случае получается, что сначала они будут инициализированы, а потом им присваивается значение.
	Актуально для 'тяжелых' переменных класса - для простых типов не актуально
	
▪ 'Следить за memory alignment объектов'
	class T1 {                                      	class T2 {
		char a;                                    		   char a;
		char b;       [sizeof(T) = 8 ]		               int v;       [sizeof(T) = 12 ]
		int v;                                             char b;
	};                                              	};

============================================================================================================================================================	
								                    Best practices:
============================================================================================================================================================	
// Jason Turner: https://github.com/cpp-best-practices/cppbestpractices/blob/master/00-Table_of_Contents.md
============================================================================================================================================================


 ▪ 'Make interfaces precisely and strongly typed' ['Use Stronger Types']

		blink_led(unsigned long time_to_blink)  ===>  blink_led(milliseconds time_to_blink)  
		socket(int t, int p , int d)            ===>  socket(type t, protocol p, domain d);

 ▪ 'Avoid “magic constants”; use symbolic constant'                                // months are indexed 1..12
                                                                                   constexpr int first_month = 1;
	 for (int m = 1; m <= 12; ++m)   /* don't: magic constant 12 */  ====>         constexpr int last_month = 12;
		cout << month[m] << '\n';                                                  for (int m = first_month; m <= last_month; ++m)   // better

 ▪ static_assert() ....... use it 
 ▪ 'What cannot be checked at compile time should be checkable at run time'
		template <class T>
		class SimpleAllocator {
			static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");
		}
		
 ▪ '[[nodiscard]]'
   Для методов возврат от которых не нужно терять + [[nodiscard]] для классов и конструкторов 

 ▪ '[[nodiscard]]'
   Для ENUM-ов типа статуса или кода ошибок:  
   enum class [[nodiscard]] Error { OK, NOTFOUND, SOMETHING_ELSE}      

   		или   

   struct [[nodiscard]] ErrorCode{};

 ▪ '[[nodiscard]]' для конструкторов класса:

	    class Object {
	    public:
	        [[nodiscard("Please do not discard Object objects too.")]]
	        Object() {}
	    };

 ▪ Prefer {} initialization over = unless you have a strong reason not to.
	  unsigned m_value_2 { -1 }; // narrowing from signed to unsigned not allowed, leads to a compile time error


 ▪ 'Avoid Boolean input parameters' ===> use enum ins

 ▪ 'Assigning default values with brace initialization'

	{
	private:
		int m_value{ 0 }; // allowed
		unsigned m_value_2 { -1 }; // narrowing from signed to unsigned not allowed, leads to a compile time error
	}
	  
 ▪ 'By default, declare single-argument constructors explicit'

 ▪ 'No destructor is always better when it’s the correct thing to do'. Empty destructors can destroy performance:

	- They make the type no longer trivial
	- Have no functional use
	- Can affect inlining of destruction
	- Implicitly disable move operations    <---- !!!!!!! Важно

 ▪ 'Prefer user-defined literals when available' [// https://www.youtube.com/watch?v=30NFjhX__mM]

 	Так как при использовании "text"sv к примеру мы уже на этапе компиляции 100% знаем длину - улучшает производительность
	  
 ▪ 'Не отделять инициализацию от присваивания': [Do not separate Construction from Assignment]

	 const std::string str  = "1234"; // GOOD    --> BAD --->      std::string str; 
	 															   std = "123";    // BAD
 ▪ 'Prefer using over typedef for defining aliases'

	 using PFI2 = int (*)(int);      |    template<typename T>
									 |	  using PFT2 = int (*)(T);   // OK

 ▪ Consider using final for all methods you don’t plan on overriding.
   (компилятор может очень хорошо оптимизировать вызов)
	
 ▪ 'Prefer ranged-for Loop Syntax Over Old Loops:' Use auto in ranged for loops

	   for(const auto &element : container) {
			// eliminates both other problems && no possible accidental conversion
	   }

 ▪ 'Follow the Rule of Zero:'

 	No destructor is always better when it’s the correct thing to do. Empty destructors can destroy performance:
	• They make the type no longer trivial
	• Have no functional use
	• Can affect inlining of destruction
	• Implicitly disable move operations


 ▪ 'If You Must Do Manual esource Management Follow the Rule of 5'

 ▪ 'Ensure that a copyable class has a default constructor'

 ▪ 'Explicitly disallow the use of compiler-generated functions you do not want'

---- Const & constexpr:

 ▪ Делаем все const что можем сделать const ['Const as Much as Possible'] --> (Communicates meaning to the readers of our code.)

     const std::string str1 = "val1"; 
	 const std::string str2 = "val2";
	 const std::string str3 = str1 + str2; // в ASM коде будет всего одна инструкция, Все будет оптимизированно

 ▪ constexpr Everything Known at Compile Time

 ▪ By default, make member functions const

 		A member function should be marked const unless it changes the object’s observable state.
 		This gives a more precise statement of design intent, better readability, more errors caught by the compiler, and sometimes more optimization opportunities.

 ▪ Prefer if constexpr over SFINAE

 ▪ 'Prefer compile-time checking to run-time checking'

 	static_assert(sizeof(Int) >= 4);    // do: compile-time check
 	void read(span<int> r)				// read into the range of integers r

 ▪ If it is not possible to user IF CONSTEXPR --> use STATIC_ASSERT (with good comments)

	
 ▪ Use lambdas for complex initialization, especially of const variables
	const widget x = [&] {
		widget val;                                 // assume that widget has a default constructor this
		for (auto i = 2; i <= N; ++i)               // could be some arbitrarily long code needed to initialize x
			val += some_obj.do_something_with(i);                                         
		return val;
    }();

 ▪ Use lambdas to intialize inline static  variables of the class
	class MyClass final {
		inline static int default_height {[] {
			std::cout << "Calling lambda to init class static variable111!\n";
			return 10;
		}()} ;
	};

	'NOTE': This will lambda be called in any case!!! Независимот от того создали ли  мы объект класса или нет

 ▪ Use std::call_once() to implemenat Singleton

    Или во всех случаях где нужно что бы какая то переменная класса потокобезопастно инциализировалась всего раз
		
 ▪ Mutex and mutable go together
	
 ▪ A class with a virtual function should have a virtual or protected destructor

    Класс предназначенный для наслодования должен иметь виртуальный деструктор!!!

 ▪ Avoid trivial getters and setters

	Reason A trivial getter or setter adds no semantic value; the data item could just as well be public.
	
 ▪ For making deep copies of polymorphic classes prefer a virtual clone function instead of copy construction/assignment
	  
 ▪ Ссылки или указатели

	Старайтесь использовать ссылки, а не указатели --> Ссылки не требуют проверок. 
	Ссылка непосредственно указывает на объект, а указатель содержит адрес, который нужно прочитать.
	
 ▪ Using unnamed namespaces instead of static globals


---- Memory:

 ▪ Prefer scoped objects, don’t heap-allocate unnecessarily [use a local variables on stack if needed]
 ▪ Avoid calling new and delete explicitly [*** No More new ***]
 
 ▪ 'Perform at most one explicit resource allocation in a single expression statement'
	// BAD: potential leak
	fun(shared_ptr<Widget>(new Widget(a, b)), shared_ptr<Widget>(new Widget(c, d)));
	
 ▪ Use std::unique_ptr<T> or std::shared_ptr<T> to represent ownership
 
 ▪ Prefer unique_ptr over shared_ptr unless you need to share ownership
   // Reason A unique_ptr is conceptually simpler and more predictable (you know when destruction happens) 
   // and faster (you don’t implicitly maintain a use count).
 
 ▪ Take a unique_ptr<widget>& parameter to express that a function reseats thewidget
 
 ▪ Memory: Never throw while being the direct owner of an object


 ▪ Never Use std::bind
   // std::bind is almost always way more overhead (both compile time and runtime) than you need. Instead simply use a lambda.

	auto f = std::bind(&my_function, "hello", std::placeholders::_1);       // <------ Bad
	auto f = [](const std::string &s) { return my_function("hello", s); };  // <------ Good 
																								cout << month[m] << '\n';

 ▪ 'make a class final and the object immutable where possible'.
 
 ▪ 'Dont return std::string_view'

 ▪ 'Use class if the class has an invariant; use struct if the data members can vary independently


============================================================================================================================================================	
								              Best practices: [Performance]
============================================================================================================================================================	

 • Avoid excessive copying:

 	Everyday coding:

 	-	Inside functions, pass bigger objects by reference, not by value
 	-	Prefer a copy constructor to an empty constructor followed by an assignment
 	-	Reuse old variables instead of constructing new ones ???
 	-	Prefer compound assignment operators to arithmetic and logical operators
 			'result = result & a;'  -->  'result &= a;'
 			'result = result | b;'	-->  'result |= b;'

 	Class design:

 	-	Inside a constructor, initialize the member variables in the constructor initialize list
 	-	Define move constructor and move assignment operator
 	-	Mark constructors as explicit
 	-	Take advantage of copy elision

 	Containers:

 	-	Prefer pre-increment operator on the iterators
 	-	Reserve space in containers
 	-	Use references in for range loops
 	-	Construct directly into the containers : emplace_back()

 • 'Use your data while still in cache'                int * a = initialize_array(size);               // Ideally we would like to load data from the memory to the 
 													   int min = a[0];								   // cache exactly once, do some modification on it, and then return 
 	int * a = initialize_array(size);				   int max = a[0];	                               // them back to the operating memory. If you need to fetch the same data two times, 
	int min = find_min(a, size);        --->           for (int i = 0; i < size; i++) {                // you are not using the cache optimally.	
	int max = find_max(a, size);                            min = std::min(a[i], min);
															max = std::max(a[i], max);
														}
 • Grouping function arguments:

 	In a loop that calls a function with more arguments than there are registers, consider passing a struct or object instead.

 • Place related data close in memory to allow efficient caching – the principle of locality

 • Understand how cache lines work

 • Use appropriate data structures

 • Avoid unpredictable branches

 • Осторожнее с делением и % | определения остатка от деления лучше попробовать воспользоваться бит-операциями (деление трудоёмко)

 • std::atomic: - не вызвать несколько раз последовально load() операции                
                                                                                         const int val = atomic_var.load();      
 		int a = atomic_var;       'и не'    int a = atomic_var.load();     |  -->  |     int a = val; 
 		if (atomic_var > XXX)               if (atomic_var.load() > XXX)   |       |     if (val > XXX) {}

 • Avoid virtual functions | 'вообще это вопрос для исследования - virtual не вносят какого-либо overhead-а cейчас почти'

 • Avoid false sharing problem

 • Links:
 	
 	'On harmful overuse of std::move' # https://devblogs.microsoft.com/oldnewthing/20231124-00/?p=109059

============================================================================================================================================================	
								              Best practices: [Multithreading] [Performance]
============================================================================================================================================================	

 ▪ Use std::async instead of using threads to simulate asynchronous operation. Especially for one-shot asynchronous task

 ▪ Small workloads to not profit from multithreading. [Performance]
   Spawning threads is costly, and so is synchronization.
   Sorting an array of 100 elements will not be faster with multithreading.
   If you are looking for ways to speed up small workloads, vectorization is a much safer bet.

 ▪ avoid thread synchronization as much as possible. [Performance]
   Every time two threads needs to content for a memory location or any other resource, one thread will have to wait, which in the essence means reverting back to serial execution.
   A shared variable protected by a mutex and accessed by all the threads all the time is a definite performance killer

============================================================================================================================================================
								              Code Review Checklist Items:                                                                               
============================================================================================================================================================

 ▪ BAD practices & Code Smells [Следующий блок чуть ниж]

 ▪ 75% разработчик НЕ пишет код а ковыряется в чужом коде - ПИШИ хороший код

 ▪ Functionality is implemented in a simple, maintainable, and reusable manner.
    [SOLID, KISS, DRY,  A PIE]

 ▪ Use of descriptive and meaningful variable, method and class names as opposed to relying too much on comments.
		Bad:  std::vector<std::string> list;
		Good: std::vector<std::string> users;

 ▪ Class and functions should be small and focus on doing one thing. No duplication of code.
 ▪ Functions should not take too many input parameters. [2 - 3 MAX]
 ▪ Declare the variables with the smallest possible scope.
 ▪ Don’t preserve or create variables that you don’t use again.
 ▪ Keep synchronization section small and favor the use of the new concurrency libraries to prevent excessive synchronization
 ▪ Never call unknown code while holding a lock (e.g., a callback)
 ▪ Reuse objects via flyweight design pattern.
 ▪ Favor using well proven frameworks and libraries as opposed to reinventing the wheel by writing your own.
 
 ▪ Presence of hard coded config values.
   [Externalize configuration data in a .properties file. Sensitive information like password must be encrypted.]

 ▪ Make a class final and the object immutable where possible.
   Immutable classes are inherently thread-safe and more secured. For example, the Java String class is immutable and declared as final.
 
 ▪ Use right data types: [Use enums instead of int constants.]

 ▪ Do not cast away const
 
 ▪ Return the right type from functions:
   if (b) ? "OK" : "False";                            // BAD. 
   if (b) ? srtd::string("OK") : std::string("False"); // GOOD
   
 ▪ Masic numbers (reserveMemory(4096)); // BAD
   or // comments with magic numbers
   // OK  use constexpr  
	
============================================================================================================================================================	
								           BAD practices & Code Smells
============================================================================================================================================================

 ▪ Construction separated from Assignment
 ▪ Destructor doing nothing (they disablee move)
 ▪ Out Variables
 ▪ Boolean input parameters (shall be replaced with enum)
 ▪ Raw Loops
 ▪ Multi-Step Function
 ▪ Code With Conversation
 ▪ Casts         ----> minimize their use; templates can help
 ▪ Casts Away CONST
 ▪ static const
 ▪ extern const
 ▪ Raw new and delete
 ▪ NON const global something / variables
 ▪ Never call unknown code while holding a lock (e.g., a callback)
 ▪ Masic numbers (reserveMemory(4096));
 ▪ Using unions  ----> use variant (in C++17)
 ▪ Array decay   ----> use std::span<T>
 ▪ Range errors  ----> use std::span<T>

============================================================================================================================================================	
								                    Curious Bugs
============================================================================================================================================================

 ▪ std::map<>::operator[]
																			void Widget::configure(const std::map<std::string, std::string>& setting) {
		std::map<string, int> dict;                                              m_timeout = settings["timeout"];  /* Set to 0 if 'timeout' missing */
		dict["hey"] = 12;													     m_size = settings["max_items_count"];	
		std::cout << dict["hye"] << std::endl; /* --> RETURN 0  */           } 
																		 
 ▪ std::shared_ptr<T>  // IS NOT THREAD SAFE
 
 ▪ Bit shift:
		unsigned int putBist(unsigned int v, int pos) {
			int right = 32 - pos;
			val = val >> right; // IF 'right' become 0 or negatinve. The result is unspecified behaviour
			return val;         // The result is COMPILLER DEPENDENT
		}
 
 ▪ Unexpected INLINING & Release builds:
 
		SomeValue theCrashyFunction(SomeParam& param) {
			AList* pList = nullptr;
			initializeList(&pList);                           void handle_struct() { /* This could cause a buffer overflow! */
	                                                              char dBuf[4 * sizeof(long unsigned)];
			aStruct->mList = pList;                               sprintf(dBuf, “\,,/(^_^)\,,/-%lx”,(long unsigned) dpt);
			if (cond)                                         }
				handle_struct(aStruct);                    // Compiler optimiser in the RELEASE build could INLINE 'handle_struct' function
                                                           // right to 'theCrashyFunction' and as the result 'sprintf' could overwrite the memory of 'pList'
			unsigned int size = pList->size();             // variable on the stack
		}
  
 ▪ Logger

 		struct Logger
 		{

        	std::jthread logProcessor;
        	std::vector<std::shared_ptr<ILogHandler>> handlers;

	        Logger() {
				logHandler = std::jthread(&Logger::handleLogs, this, stopSource);
	        }

        	void handleLogs(const std::stop_source& source)
        	{   ...
        	    for (const auto& entry: logsLocal)
                {
                    for (const std::shared_ptr<ILogHandler>& handler: handlers) {
                        handler->handleEntry(entry);
                    }
                }
        	}
 		}


 	    // При вызове дестуктора класса Logger
 	    // Сначала будет вызван дестуктор для "std::vector<std::shared_ptr<ILogHandler>> handlers"
 	    // и потом дестуктор "std::jthread logProcessor"
 	    // В итогe в методе handleLogs() при попытке итерации по Logger::handlers все упадёт
                                           struct Logger
                                           {
 												 std::vector<std::shared_ptr<ILogHandler>> handlers;
 	    ---> Фикс поменять очередность на    	 std::jthread logProcessor;
        	                               }


============================================================================================================================================================	
								                    Secure coding | C++ Hardening | Sanitizing C++ | Безопастность
============================================================================================================================================================

// [RFC] Hardening in libc++ | https://discourse.llvm.org/t/rfc-hardening-in-libc/73925?utm_source=substack&utm_medium=email
// https://discourse.llvm.org/t/rfc-c-buffer-hardening/65734/8

• GCC flag -fhardened    /**  It covers a set of safety flags that address specific attacks and library-level checks (for both glibc and stdlibc++). **/

	-D_FORTIFY_SOURCE=3
	-D_GLIBCXX_ASSERTIONS
	-ftrivial-auto-var-init=zero
	-fPIE  -pie  -Wl,-z,relro,-z,now
	-fstack-protector-strong
	-fstack-clash-protection
	-fcf-protection=full (x86 GNU/Linux only)


• Three protections for specific attacks

	-fstack-protector-strong
	-fstack-clash-protection
	-fcf-protection=full

============================================================================================================================================================	
								                    Performance : Branch - Removal
============================================================================================================================================================

void filterNegative(vector<int>& values)                  |         void filterNegativeOpt(vector<int>& values)
{								                          |			{
	auto j = 0;			                                  |				 auto j = 0;
  	for(auto i = 0; i < values.size(); i++)			      |				 for(auto i = 0; i < values.size(); i++) {
    	if (values[i] < 0) values[j++] = values[i];		  |				 	values[j] = values[i];
  	values.resize(j);			                          |					j += values[i] < 0;   /// <--- No more conditional Jump here
}				                                          |				 }
														  |