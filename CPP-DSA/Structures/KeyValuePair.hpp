#pragma once

namespace DSA {

	namespace concepts {

		template <typename T>
		concept KeyValuePair = requires(T t) {

			typename T::KEY;
			typename T::VALUE;

			t.key;
			t.value;

		};

	}

	template <typename KEY_, typename VALUE_>
	struct KeyValuePair {

		using KEY = KEY_;
		using VALUE = VALUE_;

		KEY key;
		[[no_unique_address]] VALUE value;

		constexpr KeyValuePair(KEY newKey) : key(newKey) {}
		constexpr KeyValuePair(KEY newKey, VALUE newValue) : key(newKey), value(newValue) {}

	};

	template <typename KEY_>
	struct KeyValuePair<KEY_, void> {

		using KEY = KEY_;
		using VALUE = void;

		KEY key;

		constexpr KeyValuePair(KEY newKey) : key(newKey) {}

	};

}