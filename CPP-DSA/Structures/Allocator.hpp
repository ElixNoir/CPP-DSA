#pragma once

#include <concepts>
#include <cstdint>

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

	template <typename T_ = void, std::unsigned_integral INDEX_ = uint32_t, bool IGNORE_FAILED_ALLOCATION_ = false>
	struct AllocatorConfiguration {

		using INDEX = INDEX_;
		using T = T_;

		constexpr static bool IGNORE_FAILED_ALLOCATION = IGNORE_FAILED_ALLOCATION_;

	};

	template <concepts::AllocatorConfiguration Configuration = AllocatorConfiguration<>>
	class EMPTY_ALLOCATOR {
	public:

		using INDEX = Configuration::INDEX;
		using T = Configuration::T;

		constexpr static bool IGNORE_FAILED_ALLOCATION = Configuration::IGNORE_FAILED_ALLOCATION;

		constexpr static T* allocate(INDEX size) noexcept {
			return nullptr;
		}

		constexpr static void deallocate(T* block) noexcept {}

		constexpr static T* reallocate(T* block, INDEX size) noexcept {
			return block;
		}

	};

}
