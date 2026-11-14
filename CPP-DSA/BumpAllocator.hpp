#pragma once

#include "Collection.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class BumpAllocator : Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Constructors & Destructors

		~BumpAllocator() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				T* const data = Base::get_data();
				destroy_range_forward(data, data + Base::get_size());
			}
		}

#pragma endregion

#pragma region Methods

#pragma region Allocate

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			return Base::can_add(count, explicitCapacity);
		}

		[[nodiscard]] constexpr INDEX allocate() noexcept {
			return ++Base::size;
		}

		[[nodiscard]] constexpr INDEX allocate(size_t count) noexcept {
			INDEX index = Base::get_size();
			Base::size += count;
			return index;
		}

#pragma endregion

#pragma region Memory Management

#pragma endregion

#pragma endregion

	};

}