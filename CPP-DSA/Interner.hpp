#pragma once

#include "DSA.hpp"
#include "PointerHashmap.hpp"

#include <memory_resource>
#include <string_view>
#include <unordered_set>
#include <cstring>

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, concepts::Hashable T>
	class Interner {
	public:

		using INDEX = typename ContainerPolicy::INDEX;

	protected:

		std::pmr::monotonic_buffer_resource arena; // need to change this to a genuine arena
		PointerHashmap<ContainerPolicy, T> map;

	public:

		INDEX add(const T& key) {
			INDEX index = map.find_unoccupied(&key);
			if (index == map.get_capacity())
				return index;

			T* destination = reinterpret_cast<T*>(arena.allocate(sizeof(T), alignof(T)));
			copy_construct_at(destination, key);
			map.insert_at(destination, index);

			return index;
		}

		INDEX upsert(const T& key) {
			INDEX index = map.find_occupied(&key);
			if (index != map.get_capacity())
				return index;
			else {
				T* const destination = reinterpret_cast<T*>(arena.allocate(sizeof(T), alignof(T)));
				copy_construct_at(destination, key);
				return map.insert(destination);
			}
		}

		//void remove_at(INDEX index) noexcept {
			//destroy_at();
		//}

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