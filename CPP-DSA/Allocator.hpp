#pragma once

#include <concepts>

namespace DSA {

	template <typename _T = void, std::unsigned_integral _INDEX = size_t>
	class EMPTY_ALLOCATOR {
	public:

		using T = _T;
		using INDEX = _INDEX;

		static T* allocate(INDEX size) {
			return nullptr;
		}

		static void deallocate(T* block) noexcept {}

		static T* reallocate(T* block, INDEX size) {
			return block;
		}

	};

	namespace concepts {

		template <typename T>
		concept Allocator = requires(T allocator, typename T::T* block, typename T::INDEX size) {

			typename T::T;
			typename T::INDEX;

			{ allocator.allocate(size) } -> std::same_as<typename T::T*>;
			{ allocator.deallocate(block) };

		};

		template <typename T>
		concept ReallocatableAllocator = Allocator<T> && requires(T allocator, typename T::T* block, typename T::INDEX size) {

			{ allocator.reallocate(block, size) } -> std::same_as<typename T::T*>;

		};

		template <typename T>
		concept NothrowResizableAllocator = Allocator<T> && requires(T allocator, typename T::T* block, typename T::INDEX size) {
			requires (
				(ReallocatableAllocator<T> && noexcept(allocator.reallocate(block, size)))
				|| (!ReallocatableAllocator<T> && noexcept(allocator.allocate(size) && noexcept(block)))
			);
		};

	}

}