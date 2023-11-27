/**============================================================================
Name        : Filter_ColorSize.cpp
Created on  : 27.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Filter pattern src (ColorSize)
============================================================================**/

#include <iostream>
#include <vector>

namespace Filter_ColorSize
{
    enum class Color {
        Red,
        Green,
        Blue
    };

    enum class Size {
        Small,
        Medium,
        Large
    };

    struct Product
    {
        std::string name;
        Color color;
        Size size;
    };

    template <typename T>
    struct AndSpecification;

    template <typename T>
    struct Specification
    {
        virtual bool isSatisfied(T* item) const noexcept = 0;

        AndSpecification<T> operator && (Specification&& other)
        {
            return AndSpecification<T>(*this, other);
        }
    };

    template <typename T>
    AndSpecification<T> operator && (Specification<T>& first, Specification<T>& second)
    {
        return AndSpecification<T>(first, second);
    }

    template <typename T>
    struct Filter
    {
        virtual std::vector<T*> filter(std::vector<T*> items, Specification<T>& spec) = 0;
    };

    struct ProductFilter : Filter<Product>
    {
        std::vector<Product*> filter(std::vector<Product*> items,
                                     Specification<Product>& spec) override
        {
            std::vector<Product*> result;
            for (auto& p : items)
                if (spec.isSatisfied(p))
                    result.push_back(p);
            return result;
        }
    };

    template <typename T>
    struct AndSpecification : Specification<T>
    {
        // TODO: To vector?
        Specification<T>& first;
        Specification<T>& second;

        AndSpecification(Specification<T>& first, Specification<T>& second):
                first { first }, second { second } {
            // std::cout << "FIXME\n";
            std::cout << "FIXME" << std::endl;
        }

        // TODO: To std::all ?
        bool isSatisfied(T* item) const noexcept override
        {
            return first.isSatisfied(item) && second.isSatisfied(item);
        }
    };

    struct ColorSpecification : Specification<Product>
    {
        Color color;

        explicit ColorSpecification(Color color) : color { color } {
        }

        bool isSatisfied(Product* item) const noexcept override {
            return item->color == color;
        }
    };

    struct SizeSpecification : Specification<Product>
    {
        Size size;

        explicit SizeSpecification(Size size) : size { size } {
        }

        bool isSatisfied(Product* item) const noexcept override {
            return item->size == size;
        }
    };


}

void Filter_ColorSize_Test()
{
    using namespace Filter_ColorSize;

    Product apple{ "Apple", Color::Green, Size::Small };
    Product tree{ "Tree", Color::Green, Size::Large };
    Product house{ "House", Color::Blue, Size::Large };

    const std::vector<Product*> store { &apple, &tree, &house };

    SizeSpecification large(Size::Large);
    ColorSpecification green(Color::Green);

    AndSpecification<Product> greenAndLargeSpec { large, green };
    AndSpecification<Product> greenAndLargeSpec2 = ColorSpecification(Color::Green) && SizeSpecification(Size::Large);
    AndSpecification<Product> greenAndLargeSpec3 = large && green;


    ProductFilter prodFilter;

    /** Green **/
    {
        const std::vector<Product*> greenThings = prodFilter.filter(store, green);
        for (auto &x: greenThings)
            std::cout << x->name << " is green" << std::endl;
    }

    std::cout << "----------------------------------------------------" <<  std::endl;

    /** Green && Large **/
    {
        const std::vector<Product*> greenBigThings = prodFilter.filter(store, greenAndLargeSpec);
        for (auto& item : greenBigThings)
            std::cout << item->name << " is large and green" << std::endl;
    }

    std::cout << "----------------------------------------------------" <<  std::endl;

    // FIXME:
    /** Green && Large 2 **/
    {
        const std::vector<Product*> greenBigThings = prodFilter.filter(store, greenAndLargeSpec2);
        for (auto& item : greenBigThings)
            std::cout << item->name << " is large and green" << std::endl;
    }

    std::cout << "----------------------------------------------------" <<  std::endl;

    /** Green && Large 3 **/
    {
        const std::vector<Product*> greenBigThings = prodFilter.filter(store, greenAndLargeSpec3);
        for (auto& item : greenBigThings)
            std::cout << item->name << " is large and green" << std::endl;
    }

    std::cout << std::endl;
}