#pragma once

#include "Allocator.hpp"

#include <cstdlib>
#include <exception>

namespace DSA {

	template <concepts::AllocatorConfiguration Configuration = AllocatorConfiguration<>>
	class StandardAllocator {
	public:

		using INDEX = Configuration::INDEX;
		using T = Configuration::T;

		constexpr static bool IGNORE_FAILED_ALLOCATION = Configuration::IGNORE_FAILED_ALLOCATION;

		[[nodiscard]] static T* allocate(const INDEX size)
			noexcept (IGNORE_FAILED_ALLOCATION)
		{
			T* block = static_cast<T*>(std::malloc(size * sizeof(T)));
			if constexpr (!IGNORE_FAILED_ALLOCATION)
				if (block == nullptr)
					throw std::bad_alloc();
			return block;
		}

		static void deallocate(T* block) noexcept {
			std::free(reinterpret_cast<void*>(block));
		}

		[[nodiscard]] static T* reallocate(T* const block, const INDEX size)
			noexcept (IGNORE_FAILED_ALLOCATION)
		{
			const T* const newBlock = static_cast<T*>(std::realloc(reinterpret_cast<void*>(block), size * sizeof(T)));
			if constexpr (!IGNORE_FAILED_ALLOCATION)
				if (block == nullptr)
					throw std::bad_alloc();
			return newBlock;
		}

	};

}