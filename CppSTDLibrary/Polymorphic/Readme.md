`std::polymorphic` (C++26) стоит использовать тогда, когда тебе нужно совместить **runtime polymorphism и value semantics**.

Это фактически ответ на вопрос:

> Как хранить объекты разных производных типов через общий базовый интерфейс, но при этом иметь возможность нормально копировать их, не теряя их фактический тип?

Именно здесь `std::polymorphic` отличается от `std::indirect`.

## **1. Проблема: обычный polymorphism**

Представим базовый класс `Shape` и две реализации:

```
#include <memory>
#include <print>
#include <vector>

class Shape
{
public:
    virtual ~Shape() = default;

    virtual void draw() const = 0;
};

class Circle : public Shape
{
public:
    void draw() const override
    {
        std::println("Drawing Circle");
    }
};

class Rectangle : public Shape
{
public:
    void draw() const override
    {
        std::println("Drawing Rectangle");
    }
};
```

Мы хотим хранить разные фигуры в одном контейнере:

```
std::vector<std::unique_ptr<Shape>> shapes;

shapes.push_back(std::make_unique<Circle>());
shapes.push_back(std::make_unique<Rectangle>());
```

Это прекрасно работает.

Но возникает проблема:

```
auto copiedShapes = shapes; // Compilation error!
```

`std::unique_ptr` нельзя копировать.

Можно реализовать механизм `clone()` в базовом классе и вручную копировать каждый объект. Но это требует дополнительного кода во всех производных классах.

Для этого в C++26 появился `std::polymorphic`.

## **2. Решение:&#x20;**`std::polymorphic`

Вместо `std::unique_ptr<Shape>` используем:

```
std::vector<std::polymorphic<Shape>> shapes;
```

Полный пример:

```
#include <memory>
#include <print>
#include <vector>

class Shape
{
public:
    virtual ~Shape() = default;

    virtual void draw() const = 0;
};

class Circle : public Shape
{
public:
    explicit Circle(const int radius) : radius { radius } {}

    void draw() const override
    {
        std::println("Circle: radius = {}", radius);
    }

    int radius;
};

class Rectangle : public Shape
{
public:
    Rectangle(const int width, const int height) :
        width { width }, height { height } {}

    void draw() const override
    {
        std::println("Rectangle: {} x {}", width, height);
    }

    int width;
    int height;
};

void demonstrate()
{
    std::vector<std::polymorphic<Shape>> shapes;

    shapes.emplace_back(std::in_place_type<Circle>, 10);
    shapes.emplace_back(std::in_place_type<Rectangle>, 20, 30);

    std::println("Original shapes:");

    for (const auto& shape : shapes)
    {
        shape->draw();
    }

    // Deep copy, preserving dynamic types.
    auto copiedShapes = shapes;

    std::println("\nCopied shapes:");

    for (const auto& shape : copiedShapes)
    {
        shape->draw();
    }
}

int main()
{
    demonstrate();
}
```

Результат:

```
Original shapes:
Circle: radius = 10
Rectangle: 20 x 30

Copied shapes:
Circle: radius = 10
Rectangle: 20 x 30
```

Самое интересное здесь:

```
auto copiedShapes = shapes;
```

При копировании:

1. Создаются новые динамические объекты.
2. Сохраняются фактические типы (`Circle`, `Rectangle`).
3. Копируются данные производных классов.
4. Объекты остаются независимыми.

То есть `std::polymorphic` автоматически обеспечивает polymorphic deep copy.

## **3.&#x20;**`std::indirect`**&#x20;vs&#x20;**`std::polymorphic`

Это самое важное различие.

\*\*std::indirect

Сравним на практике:

| Возможность                             | `unique_ptr<Base>` | `indirect<T>`          | `polymorphic<Base>` |
| --------------------------------------- | ------------------ | ---------------------- | ------------------- |
| Динамическое размещение                 | Да                 | Да                     | Да                  |
| Уникальное владение                     | Да                 | Да                     | Да                  |
| Копирование                             | Нет                | Да                     | Да                  |
| Deep copy                               | Вручную            | Да                     | Да                  |
| Runtime polymorphism                    | Да                 | Нет, не его назначение | Да                  |
| Сохранение derived-типа при копировании | Вручную            | Не гарантируется       | Да                  |

## **4. Когда что использовать?**

Я бы придерживался следующего правила:

`std::indirect<T>`

Когда тип объекта известен, но ты хочешь динамическое размещение и value semantics.

Например:

```
std::vector<std::indirect<Data>> items;
```

`std::polymorphic<Base>`

Когда конкретный тип объекта определяется во время выполнения и может быть разным.

Например:

```
std::vector<std::polymorphic<Shape>> shapes;
```

`std::unique_ptr<Base>`

Когда нужен runtime polymorphism, но копирование объектов не требуется или реализуется отдельно.

Например:

```
std::vector<std::unique_ptr<Shape>> shapes;
```