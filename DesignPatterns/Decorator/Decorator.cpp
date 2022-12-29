#include "Decorator.h"

void Component::operation() {
	std::cout << "World!" << std::endl;
}



DecoratorOne::DecoratorOne(std::shared_ptr<IComponent> component) : m_component(component) {
}

void DecoratorOne::operation() {
	std::cout << "...... ";
	m_component->operation();
}

DecoratorTwo::DecoratorTwo(std::shared_ptr<IComponent> component) : m_component(component) {
}

void DecoratorTwo::operation() {
	std::cout << "Hello";
	m_component->operation();
}



void DecoratorTests::Test() {
	DecoratorTwo obj(std::make_shared<DecoratorOne>(std::make_shared<Component>()));
	obj.operation(); 
}
