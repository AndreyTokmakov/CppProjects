//============================================================================
// Name        : Optional.h
// Created on  : 13.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Optional (custom implementation) src
//============================================================================

#ifndef OPTIONAL_CUSTOM_TESTS__H_
#define OPTIONAL_CUSTOM_TESTS__H_

#include <iostream>
#include <string>
#include <memory>
#include <cstring>

namespace Optional {

    /*
    template<typename T>
    union storage_t {
        using aligned_storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
        aligned_storage_t data;

        storage_t() = default;

        T* as() { return reinterpret_cast<T*>(&data); }
        // use placement new to create an instance of T inside this union
    };
    */

	template<typename Ty>
	class Optional {
	private:
		using data_type = Ty;
		using pointer   = data_type*;

		/*
		static_assert(!std::is_same_v<KeyType, void>, "ERROR: Key type can not be void");
		static_assert(0 != _Size,  "ERROR: Please try a little bigger buffer");
		*/

	private:
		char data [sizeof(data_type)]{};
		bool has_value { false };

	public:
		Optional() noexcept = default;

		template<typename ... Types>
		Optional(Types&& ... params) {
			new (data) data_type(std::forward<Types>(params)...);
			has_value = true;
		}

		Optional(data_type&& obj) noexcept {
			memmove(data, &obj, sizeof(data_type)); // TODO: BaD!!!
			memset(&obj, 0, sizeof(data_type));
			has_value = true;
		}

		~Optional() {
			if (!has_value) {
				// Nothing to destroy.
				return;
			}

			/* Call controlles object destructor. (Case 1) */
			// (reinterpret_cast<pointer>(&data))->~data_type();

			/* Call controlles object destructor. (Case 2)  */
			auto ptr = std::launder(reinterpret_cast<pointer>(data));
			std::destroy_at(ptr);
		}

		inline explicit operator bool() const noexcept {
			return has_value;
		}

		[[nodiscard]]
        inline bool hasValue() const noexcept {
			return has_value;
		}
	};

	/** Test go there. **/
	void TEST_ALL();
};

#endif // (!OPTIONAL_CUSTOM_TESTS__H_)