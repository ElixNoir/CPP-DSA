#pragma once

#include "Collection.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class BumpAllocator : Collection<ContainerConfiguration, T> {
	public:

		using Base = Collection<ContainerConfiguration, T>;
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

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1, INDEX capacity = Base::default_capacity()) const noexcept {
			return Base::can_add(capacity, count);
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