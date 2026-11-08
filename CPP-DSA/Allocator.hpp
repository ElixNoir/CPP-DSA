#pragma once

#include <concepts>

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Allocator = requires(T allocator, typename T::T* block, typename T::INDEX size) {

			typename T::INDEX;
			typename T::T;

			{ allocator.allocate(size) } -> std::same_as<typename T::T*>;
			{ allocator.deallocate(block) };

		};

		template <typename T>
		concept AllocatorConfiguration = requires(T t) {
			
			typename T::INDEX;
			typename T::T;

			{ T::IGNORE_FAILED_ALLOCATION } -> std::same_as<const bool&>;

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

	template <typename _T = void, std::unsigned_integral _INDEX = size_t, bool _IGNORE_FAILED_ALLOCATION = false>
	struct AllocatorConfiguration {

		using INDEX = _INDEX;
		using T = _T;

		constexpr static bool IGNORE_FAILED_ALLOCATION = _IGNORE_FAILED_ALLOCATION;

	};

	template <concepts::AllocatorConfiguration Configuration = AllocatorConfiguration<>>
	class EMPTY_ALLOCATOR {
	public:

		using INDEX = typename Configuration::INDEX;
		using T = typename Configuration::T;

		constexpr static bool IGNORE_FAILED_ALLOCATION = Configuration::IGNORE_FAILED_ALLOCATION;

		static T* allocate(INDEX size) noexcept {
			return nullptr;
		}

		static void deallocate(T* block) noexcept {}

		static T* reallocate(T* block, INDEX size) noexcept {
			return block;
		}

	};

}