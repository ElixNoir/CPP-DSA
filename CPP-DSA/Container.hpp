#pragma once

#include "Allocator.hpp"
#include "StandardAllocator.hpp"
#include "Storage.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Container = Storage<T> && HasCapacity<T>;

		template <typename T>
		concept ContainerPolicy = StoragePolicy<T>;

		template <typename T>
		concept DynamicContainer = DynamicStorage<T> && HasCapacity<T>;

		template <typename T>
		concept ResizableContainer = DynamicContainer<T> && requires(T t, size_t newCapacity) {

			{ t.resize(newCapacity) };

		};

	}

	template <concepts::StoragePolicy STORAGE_POLICY>
	struct ContainerPolicy;

	template <concepts::DynamicStoragePolicy STORAGE_POLICY>
	struct ContainerPolicy<STORAGE_POLICY> : public STORAGE_POLICY {

		template <typename _T>
		class STRUCTURE : public STORAGE_POLICY::template STRUCTURE<_T> {
		public:

			using Base = typename STORAGE_POLICY::template STRUCTURE<_T>;
			using typename Base::ALLOCATOR;
			using typename Base::INDEX;
			using typename Base::T;

			using This = STRUCTURE<T>;

			constexpr static bool is_dynamic = Base::is_dynamic;
			constexpr static INDEX maximum_capacity = Base::maximum_capacity;
			constexpr static INDEX maximum_index = Base::maximum_index;

		protected:

			INDEX capacity = 0;

		public:

#pragma region Constructors & Destructors

			STRUCTURE() : Base() {}

			STRUCTURE(INDEX initialCapacity)
				noexcept (concepts::NothrowResizableStorage<Base, T>)
				requires Base::is_dynamic
			: Base(initialCapacity), capacity(initialCapacity) {}

			STRUCTURE(std::initializer_list<T> initializerList) noexcept : Base(initializerList), capacity(initializerList.size()) {}

			STRUCTURE& operator=(const STRUCTURE& other)
				noexcept (concepts::NothrowResizableStorage<Base, T>)
				requires (Base::is_dynamic && std::is_trivial_v<T>)
			{
				const INDEX capacity = get_capacity();
				const T* const data = Base::get_data();
				const INDEX otherCapacity = other.get_capacity();
				const T* const otherData = other.get_data();

				if (capacity >= otherCapacity)
					std::memcpy(data, otherData, capacity);
				else {
					Base::no_move_resize(otherCapacity);
					std::memcpy(data, otherData, capacity);
				}
			}

			STRUCTURE& operator=(const STRUCTURE& other)
				requires (!Base::is_dynamic && std::is_trivial_v<T>)
			{
				std::memcpy(Base::get_data(), other.get_data(), get_capacity());
			}

			STRUCTURE(const STRUCTURE& other)
				noexcept (concepts::NothrowResizableStorage<Base, T>)
				requires (Base::is_dynamic && std::is_trivial_v<T>)
			: Base(other.get_capacity()), capacity(other.get_capacity()) {
				std::memcpy(Base::get_data(), other.get_data(), other.get_capacity());
			}

			STRUCTURE(const STRUCTURE& other)
				noexcept (concepts::NothrowResizableStorage<Base, T>)
				requires (!Base::is_dynamic && std::is_trivial_v<T>)
			{
				std::memcpy(Base::get_data(), other.get_data(), get_capacity());
			}

#pragma endregion

#pragma region Methods

#pragma region Getters

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return capacity;
			}

#pragma endregion

#pragma region Checks

			[[nodiscard]] constexpr bool contains_index(INDEX index) const noexcept {
				return index < get_capacity();
			}

#pragma endregion

#pragma region Memory Management

			constexpr void double_capacity()
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				const INDEX capacity = get_capacity();
				grow(capacity << 1);
			}

			constexpr void expand(float ratio = 1.5)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				const INDEX capacity = get_capacity();
				grow(Math::ceiling(capacity * ratio));
			}

			constexpr void grow(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				const INDEX capacity = get_capacity();
				T* const oldData = Base::get_data();
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					set_data(ALLOCATOR::reallocate(oldData, newCapacity));
				else {
					T* const newData = ALLOCATOR::allocate(newCapacity);
					std::memcpy(newData, oldData, capacity);
					ALLOCATOR::deallocate(oldData);
					Base::set_data(newData);
				}
			}

			constexpr void no_move_resize(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					set_data(ALLOCATOR::reallocate(Base::get_data(), newCapacity));
				else {
					ALLOCATOR::deallocate(Base::get_data());
					set_data(ALLOCATOR::allocate(newCapacity));
				}
			}

			constexpr void reserve(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				const INDEX capacity = get_capacity();
				if (newCapacity > capacity)
					grow(newCapacity);
			}

			constexpr void resize(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				const INDEX capacity = get_capacity();
				if (newCapacity > capacity)
					grow(newCapacity);
				else
					shrink(newCapacity);
			}

			constexpr void shrink(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				T* const oldData = Base::get_data();
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					Base::set_data(ALLOCATOR::reallocate(oldData, newCapacity));
				else {
					T* const newData = ALLOCATOR::allocate(newCapacity);
					std::memcpy(newData, oldData, newCapacity);
					ALLOCATOR::deallocate(oldData);
					Base::set_data(newData);
				}
			}

#pragma endregion

#pragma region Setters

			constexpr void set_capacity(INDEX newCapacity) noexcept
				requires Base::is_dynamic
			{
				capacity = newCapacity;
			}

			constexpr void one() noexcept {
				Base::one(get_capacity());
			}

			constexpr void one_raw() noexcept {
				Base::one_raw(get_capacity());
			}

			constexpr void zero() noexcept {
				Base::zero(get_capacity());
			}

			constexpr void zero_raw() noexcept {
				Base::zero_raw(get_capacity());
			}

#pragma endregion

#pragma endregion

		};

	};

	template <concepts::StaticStoragePolicy STORAGE_POLICY>
	struct ContainerPolicy<STORAGE_POLICY> : public STORAGE_POLICY {};

}