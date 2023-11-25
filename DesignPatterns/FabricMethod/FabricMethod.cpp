#include "FabricMethod.h"

namespace FabricMethod {

    struct IProduct
    {
        virtual ~IProduct() = default;
        virtual std::string getName() = 0;
    };

    struct ICreator
    {
        virtual std::unique_ptr<IProduct> factoryMethod() = 0;
    };


    struct ProductA : public IProduct
    {
        std::string getName() override {
            return "ProductA";
        }
    };

    struct ProductB : public IProduct
    {
        std::string getName() override {
            return "ProductB";
        }
    };


    class FabricOne : public ICreator {
    public:
        std::unique_ptr<IProduct> factoryMethod() final {
            return std::make_unique<ProductA>();
        }
    };


    class FabricTwo : public ICreator {
    public:
        std::unique_ptr<IProduct> factoryMethod() final {
            return std::make_unique<ProductB>();
        }
    };


	void Test1() {
        FabricOne one;
        FabricTwo two;
		ICreator* creators[] = {&one, &two};

		for (auto&& creator : creators) {
			std::unique_ptr<IProduct> product = creator->factoryMethod();
			std::cout << "New product: " <<  product->getName() << std::endl;
		}
	}

    // from FabricMethod2
    void Test2();

    void TestAll()
    {
        Test1();
        // Test2();
    }
}
