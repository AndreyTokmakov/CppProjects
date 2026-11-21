# `std::indirect`**&#x20;(C++26)**

`std::indirect` — это новый класс-шаблон из C++26, предназначенный для владения динамически выделенным объектом, но с **value semantics** (семантикой значения), а не с семантикой владения указателем, как у `std::unique_ptr`.

Он объявлен в `<memory>`.

```
template<class T, class Allocator = std::allocator<T>
>
class indirect;
```

C++26

Основная идея: объект `std::indirect<T>` хранит объект `T` динамически, но сам `indirect` ведёт себя гораздо ближе к обычному объекту, чем к указателю.

Это особенно интересно для C++ разработчика, который использует `std::unique_ptr`, контейнеры, move semantics и value semantics.

## **1. Зачем нужен&#x20;**`std::indirect`**?**

Представим обычный класс:

```
class Data
{
public:
    Data(int value) : value(value) {}

    int value;
};
```

И использование:

```
Data first { 42 };
Data second = first;
```

В результате `second` — независимая копия `first`.

**first**

value = 42

**second**

value = 42

Два независимых объекта с одинаковыми значениями.

Но иногда нам нужно, чтобы объект физически находился в динамической памяти.

Например:

- объект слишком большой;
- нужен стабильный адрес объекта;
- объект используется в контейнере, где требуется косвенное хранение;
- необходимо скрыть реализацию;
- объект должен иметь value semantics, несмотря на динамическое размещение.

Традиционно для этого используют `std::unique_ptr` или `std::shared_ptr`, однако у них другая семантика.

## **2.&#x20;**`std::unique_ptr`**&#x20;vs&#x20;**`std::indirect`

Рассмотрим привычный вариант:

```
#include <memory>

class Data
{
public:
    Data(int value) : value(value) {}

    int value;
};

int main()
{
    std::unique_ptr<Data> first = std::make_unique<Data>(42);

    // std::unique_ptr is not copyable.
    // std::unique_ptr<Data> second = first; // Error
}
```

`std::unique_ptr` обеспечивает уникальное владение. Копирование запрещено.

Чтобы создать независимую копию объекта, необходимо вручную выполнить deep copy:

```
std::unique_ptr<Data> second = std::make_unique<Data>(*first);
```

Теперь посмотрим на `std::indirect`.

```
#include <memory>

class Data
{
public:
    Data(int value) : value(value) {}

    int value;
};

int main()
{
    std::indirect<Data> first { std::in_place, 42 };

    std::indirect<Data> second = first;
}
```

Копирование разрешено.

При копировании `std::indirect` создаёт отдельный динамически размещённый объект, копируя значение исходного объекта.

**first**

Data42

**second**

Data42

Копирование indirect означает копирование объекта, а не копирование указателя.

Это принципиальное отличие от `std::unique_ptr`.

## **3. Value semantics при динамическом размещении**

Это, пожалуй, основное назначение `std::indirect`.

```
#include <memory>
#include <cassert>

class Data
{
public:
    Data(int value) : value(value) {}

    int value;
};

int main()
{
    std::indirect<Data> first { std::in_place, 42 };
    std::indirect<Data> second = first;

    second->value = 100;

    assert(first->value == 42);
    assert(second->value == 100);
}
```

Обратите внимание:

```
std::indirect<Data> second = first;
```

Это не копирование ссылки, не копирование адреса и не shared ownership.

Это копирование значения объекта `Data`.

При этом сам `Data` располагается в динамической памяти.

**Модель поведения**

**first**

42

**second**

100

Оба indirect владеют собственными объектами. Изменение одного не влияет на другой.

Таким образом, `std::indirect` позволяет реализовать обычную семантику значений, даже если объект размещён динамически.

## **4. Доступ к объекту**

Доступ осуществляется привычным образом.

### **Через&#x20;**`operator->`

```
std::indirect<Data> data { std::in_place, 42 };

data->value = 100;
```

### **Через&#x20;**`operator*`

```
Data& object = *data;

object.value = 200;
```

С точки зрения интерфейса использование очень похоже на `std::unique_ptr`.

## **5. Move semantics**

`std::indirect` поддерживает перемещение.

```
#include <memory>

int main()
{
    std::indirect<Data> first { std::in_place, 42 };

    std::indirect<Data> second = std::move(first);
}
```

После перемещения `first` может оказаться в состоянии `valueless`.

В отличие от `std::unique_ptr`, у которого после перемещения остаётся корректный объект `unique_ptr`, содержащий `nullptr`, `std::indirect` предоставляет специальную функцию для проверки состояния:

```
first.valueless_after_move();
```

Она возвращает `true`, если `indirect` находится в состоянии без принадлежащего ему объекта, в частности после перемещения.

Не следует разыменовывать valueless indirect.

## **6. Сравнение объектов**

`std::indirect` поддерживает сравнение, в том числе операторы `==` и `<=>` (C++26).

```
std::indirect<int> first { std::in_place, 42 };
std::indirect<int> second { std::in_place, 42 };

assert(first == second);
```

Здесь сравниваются значения объектов, а не адреса объектов в памяти.

Это опять-таки соответствует value semantics.

## **7.&#x20;**`std::indirect`**&#x20;и&#x20;**`std::unique_ptr`

| Свойство                | `std::unique_ptr`     | `std::indirect` |
| ----------------------- | --------------------- | --------------- |
| Динамическое размещение | Да                    | Да              |
| Уникальное владение     | Да                    | Да              |
| Копирование             | Нет                   | Да, deep copy   |
| Move                    | Да                    | Да              |
| Value semantics         | Нет                   | Да              |
| `operator->`            | Да                    | Да              |
| `operator*`             | Да                    | Да              |
| Сравнение по значению   | Не основная семантика | Да              |
| Может быть valueless    | Да (`nullptr`)        | Да (после move) |

## **8. Практический пример: контейнер**

Представим, что у нас есть структура с большим количеством данных.

```
#include <memory>
#include <vector>

class Order
{
public:
    Order(int id) : id(id) {}

    int id;
};

int main()
{
    std::vector<std::indirect<Order>> orders;

    orders.emplace_back(std::in_place, 1);
    orders.emplace_back(std::in_place, 2);
    orders.emplace_back(std::in_place, 3);

    auto copiedOrders = orders;
}
```

Теперь `copiedOrders` содержит независимые копии объектов `Order`.

Это интересная альтернатива:

```
std::vector<Order>
```

и

```
std::vector<std::unique_ptr<Order>>
```

Однако это не универсальная замена ни одному из них.

| Тип                                   | Что хранит                                         | Основная семантика |
| ------------------------------------- | -------------------------------------------------- | ------------------ |
| `std::vector<Order>`                  | Объекты непосредственно в памяти контейнера        | Value semantics    |
| `std::vector<std::unique_ptr<Order>>` | Указатели на динамические объекты                  | Unique ownership   |
| `std::vector<std::indirect<Order>>`   | Владение динамическими объектами с value semantics | Value semantics    |

Важно: `std::indirect` не гарантирует стабильность адресов самих wrapper-объектов в контейнере. Но перемещение wrapper при перемещении элементов контейнера не обязательно перемещает принадлежащий ему `Order` — это отдельный вопрос, связанный с адресом объекта и операциями над `indirect`.

## **9.&#x20;**`std::indirect`**&#x20;и polymorphism**

Это интересный момент.

Допустим, у нас есть:

```
class Base
{
public:
    virtual ~Base() = default;

    virtual void execute() = 0;
};

class Derived : public Base
{
public:
    void execute() override {}
};
```

Если мы хотим хранить полиморфные объекты, то обычно используем:

```
std::unique_ptr<Base>
```

`std::indirect<Base>` не является автоматической заменой `std::unique_ptr<Base>` для polymorphic ownership. Его копирование связано с копированием объекта типа `T`, а не с автоматическим клонированием динамического типа.

Для полиморфного value semantics в C++26 есть также `std::polymorphic` — другой wrapper, предназначенный именно для владения динамическими объектами с value semantics и сохранением динамического типа при копировании.

## **10.&#x20;**`std::indirect`**&#x20;и allocator**

`std::indirect` имеет второй параметр шаблона:

```
template<class T, class Allocator = std::allocator<T>>
class indirect;
```

Например:

```
std::indirect<Data, std::allocator<Data>> data { std::in_place, 42 };
```

Кроме того, C++26 предоставляет PMR-вариант:

```
namespace std::pmr
{
    template<class T>
    using indirect = std::indirect<T, std::pmr::polymorphic_allocator<T>>;
}
```

Это позволяет использовать `std::indirect` с `std::pmr::memory_resource`.

Для систем, где управление аллокациями важно, в частности для low-latency систем, это может быть полезным.

## **11. Когда использовать&#x20;**`std::indirect`**?**

| Сценарий                                                       | Что я бы рассматривал         |
| -------------------------------------------------------------- | ----------------------------- |
| Обычный объект                                                 | `T`                           |
| Объект в контейнере, если динамическое размещение не нужно     | `std::vector<T>`              |
| Динамический объект, уникальное владение, копирование не нужно | `std::unique_ptr<T>`          |
| Динамический объект, shared ownership                          | `std::shared_ptr<T>`          |
| Динамический объект с value semantics                          | `std::indirect<T>`            |
| Полиморфный объект с value semantics                           | `std::polymorphic<T>` (C++26) |

## **12. Что особенно интересно для C++ разработчика**

`std::indirect` решает довольно конкретную проблему языка: необходимость иметь объект, который размещается динамически, но при этом ведёт себя как значение.

До C++26, если хотелось получить подобную семантику, приходилось:

- писать собственный wrapper;
- реализовывать deep copy;
- вручную управлять copy constructor и copy assignment;
- либо использовать дополнительные библиотеки.

Теперь эта концепция становится стандартной библиотечной абстракцией.

Главная идея:

- `std::unique_ptr` — владение ресурсом и уникальная идентичность объекта.
- `std::indirect` — владение динамическим объектом, но семантика значения.
- `std::polymorphic` — динамический полиморфный объект с семантикой значения.

`std::indirect` стандартизирован в C++26. См. описание и интерфейс на cppreference.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://en.cppreference.com\&sz=32)

cppreference.com



Если интересно, могу отдельно разобрать `std::indirect` с точки зрения реализации: как можно было бы написать собственный аналог на C++20/23, включая copy constructor, allocator-aware construction и move semantics.