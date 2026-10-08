#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class IncrementalContainer : public CONTAINER {
	public:

		using Base = CONTAINER;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX size = 0;

	public:

#pragma region Constructors & Destructors

		IncrementalContainer() = default;

		using CONTAINER::CONTAINER;

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			return size;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool can_add(INDEX count = 1) const noexcept {
			return count <= Base::get_capacity() - size;
		}

		[[nodiscard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return size >= count;
		}

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return size == 0;
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			return size == Base::get_capacity();
		}

#pragma endregion

#pragma region Helpers

		constexpr void empty() noexcept requires std::is_trivial_v<T> {
			size = 0;
		}

#pragma endregion

#pragma region Setters

		constexpr void set_size(INDEX newSize) noexcept requires std::is_trivial_v<T> {
			size = newSize;
		}

#pragma endregion

#pragma endregion

	};

}