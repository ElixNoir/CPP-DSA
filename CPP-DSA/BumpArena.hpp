#pragma once

#include "Collection.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, bool ALIGN = true>
	class BumpArena : Collection<StoragePolicy, std::byte> {
	public:

		using Base = Collection<StoragePolicy, std::byte>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		//Stack<StoragePolicy, INDEX> active; // necessary only for non-explicit deallocation

	public:

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			return Base::can_add(count, explicitCapacity);
		}

		template <typename U>
		[[nodiscard]] constexpr INDEX allocate() noexcept {
			INDEX index = Base::get_size();
			
			/*
			if constexpr (ALIGN) {
				const std::byte* topAddress = Base::get_data() + index;
				const std::byte* address = align_up<U>(topAddress);
				INDEX total = address - topAddress + sizeof(U);
				Base::size += total;
			}
			else {

			}
			*/
			
			//if (index + total > Base::get_capacity()) {
				// grow
			//}
			
			return index;
		}

		template <typename U>
		constexpr void deallocate(INDEX index) noexcept {
			destroy_at<U>(Base::get_data() + index);
		}

		template <typename U>
		[[nodiscard]] constexpr INDEX padding_of(INDEX index) const noexcept {
			return align_up<U>(Base::get_data() + index) - (Base::get_data() + Base::get_size());
		}

	};

}