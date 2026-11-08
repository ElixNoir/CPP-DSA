#pragma once

#include "Allocator.hpp"

#include <cstdlib>

namespace DSA {

	template <concepts::AllocatorConfiguration Configuration = AllocatorConfiguration<>>
	class StandardAllocator {
	public:

		using INDEX = typename Configuration::INDEX;
		using T = typename Configuration::T;

		constexpr static bool IGNORE_FAILED_ALLOCATION = Configuration::IGNORE_FAILED_ALLOCATION;

		[[nodiscard]] static T* allocate(INDEX size)
			noexcept (IGNORE_FAILED_ALLOCATION)
		{
			T* block = reinterpret_cast<T*>(std::malloc(size * sizeof(T)));
			if constexpr (!IGNORE_FAILED_ALLOCATION)
				if (block == nullptr)
					throw;
			return block;
		}

		static void deallocate(T* block) noexcept {
			std::free(reinterpret_cast<void*>(block));
		}

		[[nodiscard]] static T* reallocate(T* block, INDEX size)
			noexcept (IGNORE_FAILED_ALLOCATION)
		{
			block = reinterpret_cast<T*>(std::realloc(reinterpret_cast<void*>(block), size * sizeof(T)));
			if constexpr (!IGNORE_FAILED_ALLOCATION)
				if (block == nullptr)
					throw;
			return block;
		}

	};

}