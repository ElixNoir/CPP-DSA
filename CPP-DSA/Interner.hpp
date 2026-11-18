#pragma once

#include "BuddyArena.hpp"
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

		BuddyArena<ContainerPolicy> arena;
		PointerHashmap<ContainerPolicy, T> map;

	public:

		/*
		constexpr INDEX add(const T& key) {
			const INDEX index = map.find_unoccupied(&key);
			if (index == map::invalid_index)
				return index;

			const INDEX destination = arena.allocate<T>();
			copy_construct_at(arena.get_pointer(destination), key);
			map.insert_at(arena.get_pointer(destination), index);

			return index;
		}

		constexpr INDEX upsert(const T& key) {
			const INDEX index = map.find_occupied(&key);
			if (index != map::invalid_index)
				return index;
			else {
				T* const destination = reinterpret_cast<T*>(arena.allocate(sizeof(T), alignof(T)));
				copy_construct_at(destination, key);
				return map.insert(destination);
			}
		}
		*/

		//constexpr void remove_at(INDEX index) noexcept {
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