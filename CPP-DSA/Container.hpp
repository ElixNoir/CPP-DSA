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

	template <concepts::StoragePolicy _STORAGE_POLICY>
	struct ContainerPolicy {

		using STORAGE_POLICY = _STORAGE_POLICY;

		using INDEX = typename STORAGE_POLICY::INDEX;

		template <typename T>
		using ALLOCATOR = typename STORAGE_POLICY::template ALLOCATOR<T>;

		constexpr static bool is_dynamic = STORAGE_POLICY::is_dynamic;
		constexpr static INDEX maximum_capacity = STORAGE_POLICY::maximum_capacity;
		constexpr static INDEX maximum_index = STORAGE_POLICY::maximum_index;

		template <typename _T>
		class STRUCTURE : public STORAGE_POLICY::template STRUCTURE<_T> {
		public:

			using T = _T;
			using ALLOCATOR = ALLOCATOR<T>;
			using Base = typename STORAGE_POLICY::template STRUCTURE<T>;
			using INDEX = STORAGE_POLICY::INDEX;

			using This = STRUCTURE<T>;

			constexpr static bool is_dynamic = is_dynamic;
			constexpr static INDEX maximum_capacity = maximum_capacity;
			constexpr static INDEX maximum_index = maximum_index;

		protected:

			using Capacity = Parameter<INDEX, false>;

			INDEX capacity = 0;

#pragma region Methods

			[[nodiscard]] constexpr INDEX extract_capacity(Capacity explicitCapacity = {}) const noexcept {
				return get_capacity();
			}

#pragma endregion

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
					Base::no_copy_resize(otherCapacity);
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

#pragma region Setters

			constexpr void set_capacity(INDEX newCapacity) noexcept
				requires Base::is_dynamic
			{
				capacity = newCapacity;
			}

			constexpr void one() noexcept {
				std::memset(Base::get_raw_data(), 0xFF, get_capacity() * sizeof(T));
			}

			constexpr void zero() noexcept {
				std::memset(Base::get_raw_data(), 0, get_capacity() * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};
	};

}