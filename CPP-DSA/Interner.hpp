#pragma once

#include "DSA.hpp"
#include "View.hpp"

#include <memory_resource>
#include <string_view>
#include <unordered_set>
#include <cstring>

namespace DSA {

	template <concepts::Hashable T>
	class Interner {
	protected:

		std::pmr::monotonic_buffer_resource arena;
		std::unordered_set<View> pool;

	public:

		void add(View& view) {
			if (auto iterator = pool.find(view); iterator != pool.end())
				return *iterator;

			char* buf = static_cast<char*>(arena.allocate(view.size, alignof(T)));
			std::memcpy(buf, view.data, view.size);

			view.data = buf;
			pool.insert(view);

			return view;
		}

		void remove(View& view) noexcept {
			view.data = nullptr;
		}

	};

	class StringInterner {
	protected:

		std::pmr::monotonic_buffer_resource arena;
		std::unordered_set<std::string_view> pool;

	public:

		[[nodiscard]] std::string_view intern(std::string_view string) {
			if (auto iterator = pool.find(string); iterator != pool.end())
				return *iterator;

			char* buf = static_cast<char*>(arena.allocate(string.size(), alignof(char)));
			std::memcpy(buf, string.data(), string.size());

			std::string_view interned(buf, string.size());
			pool.insert(interned);

			return interned;
		}

	};

}