#pragma once

#include "Container.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Collection = Container<T> && requires(T t, typename T::INDEX count) {

			{ t.can_add(count) } -> std::same_as<bool>;
			{ t.can_remove(count) } -> std::same_as<bool>;
			{ t.is_empty() } -> std::same_as<bool>;
			{ t.is_full() } -> std::same_as<bool>;

			{ t.get_size() } -> std::same_as<typename T::INDEX>;

		};

		template <typename T>
		concept CompleteCollection = Collection<T> && requires(
			T t,
			typename T::INDEX index,
			typename T::INDEX count,
			typename T::T value,
			typename T::T* source
		) {

			{ t.add(value) };
			{ t.add_many(source) };
			{ t.remove(index) };
			{ t.remove_many(count) };

		};

	}

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
	class Collection : public ContainerConfiguration::template STRUCTURE<T> {
	public:

		using Base = ContainerConfiguration::template STRUCTURE<T>;
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

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			return size;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool can_add(INDEX count = 1, INDEX capacity = Base::default_capacity()) const noexcept {
			return count <= capacity - size;
		}

		[[nodiscard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return size >= count;
		}

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return size == 0;
		}

		[[nodiscard]] constexpr bool is_full(INDEX capacity = Base::default_capacity()) const noexcept {
			return size == capacity;
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty() noexcept {
			size = 0;
		}

#pragma endregion

#pragma endregion

	};

}