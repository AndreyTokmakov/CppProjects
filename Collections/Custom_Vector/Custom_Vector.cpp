#include "Custom_Vector.h"

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <cassert>
#include <algorithm>
#include <chrono>

#include "../Helpers/Integer.h"

namespace Custom_Vector {

	template<typename Type >
	class Vector {
	private:
		/** Elements collection block: **/
		Type* data = nullptr;

		/** Capacity: **/
		size_t capacity = 0;

		/** Count of elements: **/
		size_t size = 0;

		/** Count of elements: **/
		size_t growth_factor = 2;

	protected:
		void validate_capacity() {
			if (size > capacity) {
				std::cout << "Need to reallocate" << std::endl;
			}
			capacity += capacity * growth_factor;
			data = new Type[this->capacity];
		}

	public:
		Vector() = default;

		Vector(size_t size): capacity(size), size(size) {
			this->data = new Type[this->capacity];
		}

		void reserve(size_t size) {
			this->capacity = size;
			this->data = new Type[this->capacity];
		}

		void push_back(Type&& v) {
			size++;
			//validate_capacity();

			this->data[size - 1] = v;
		}

		void push_back(const Type& v) {
			size++;
			//validate_capacity();

			this->data[size - 1] = v;
		}

		Type& operator[] (size_t index) {
			return this->data[index];
		}

		Type& back() {
			return this->data[this->size - 1];
		}
	};
}

void Custom_Vector::TEST_ALL() {
	Vector<int> vect;
	vect.reserve(10);

	vect.push_back(123);
	std::cout << vect[0] << std::endl;

}