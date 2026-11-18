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

	template <typename _KEY, typename _VALUE>
	struct KeyValuePair {

		using KEY = _KEY;
		using VALUE = _VALUE;

		KEY key;
		[[no_unique_address]] VALUE value;

		constexpr KeyValuePair(KEY newKey) : key(newKey) {}
		constexpr KeyValuePair(KEY newKey, VALUE newValue) : key(newKey), value(newValue) {}

	};

	template <typename _KEY>
	struct KeyValuePair<_KEY, void> {

		using KEY = _KEY;
		using VALUE = void;

		KEY key;

		constexpr KeyValuePair(KEY newKey) : key(newKey) {}

	};

}