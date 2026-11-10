#pragma once

#include "Container.hpp"
#include "Parameter.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Collection = Container<T> && requires(T t, typename T::INDEX index, typename T::T data) {

			{ t.add(data) };
			{ t.remove(index) };

			{ t.can_add(index) } -> std::same_as<bool>;
			{ t.can_remove(index) } -> std::same_as<bool>;
			{ t.is_empty() } -> std::same_as<bool>;
			{ t.is_full() } -> std::same_as<bool>;

			{ t.get_size() } -> std::same_as<typename T::INDEX>;

		};

		template <typename T>
		concept CompleteCollection = Collection<T> && requires(T t, typename T::INDEX index, typename T::T* source) {

			{ t.add_many(source) };
			{ t.remove_many(index) };

		};

	}

	template <concepts::StoragePolicy StoragePolicy, typename T>
	class Collection : public StoragePolicy::template STRUCTURE<T> {
	public:

		using Base = StoragePolicy::template STRUCTURE<T>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX size = 0;

#pragma region Methods

		constexpr void set_size(INDEX newSize) noexcept {
			size = newSize;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		Collection() : Base() {}

		Collection(INDEX initialCapacity)
			requires Base::is_dynamic
		: Base(initialCapacity) {}

		Collection(std::initializer_list<T> initializerList) noexcept : Base(initializerList), size(initializerList.size()) {}

		Collection& operator=(const Collection& other)
			requires (Base::is_dynamic && std::is_trivial_v<T>)
		{
			T* const data = Base::get_data();
			const INDEX size = get_size();
			const T* const otherData = other.get_data();
			const INDEX otherSize = other.get_size();

			if (size >= otherSize)
				std::memcpy(data, otherData, otherSize);
			else {
				Base::no_copy_resize(otherSize);
				std::memcpy(data, otherData, otherSize);
			}

			set_size(otherSize);
		}

		Collection& operator=(const Collection& other)
			requires (!Base::is_dynamic && std::is_trivial_v<T>)
		{
			std::memcpy(Base::get_data(), other.get_data(), get_size());
			set_size(other.get_size());
		}

		Collection(const Collection& other)
			noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
		: Base(other), size(other.get_size()) {}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			return size;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool can_add(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return count <= capacity - get_size();
		}

		[[nodiscard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return get_size() >= count;
		}

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return get_size() == 0;
		}

		[[nodiscard]] constexpr bool is_full(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return get_size() == capacity;
		}

#pragma endregion

#pragma endregion

	};

}