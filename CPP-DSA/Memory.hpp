#pragma once

#include "Iterator.hpp"
#include "repeat.hpp"
#include "Traits.hpp"

#include <concepts>
#include <memory>

namespace DSA {

	namespace concepts {

		template <typename T>
		concept is_nothrow_copyable_v = std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>;

		template <typename T>
		concept is_nothrow_moveable_v = std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>;

	}

}

namespace Algorithms {

	namespace Memory {

		enum class Direction { Forward, Backward };

#pragma region Alignment

		template <typename T>
		[[nodiscard]] constexpr static size_t align_up(std::byte* data) noexcept {
			return (data + alignof(T) - 1) & ~(alignof(T) - 1);
		}

		template <std::integral T>
		[[nodiscard]] constexpr static T align_up(T size, T powerOf2Alignment) noexcept {
			return (size + powerOf2Alignment - 1) & ~(powerOf2Alignment - 1);
		}

		template <std::integral T>
		[[nodiscard]] constexpr static T align_up_any(T size, T alignment) noexcept {
			return size + alignment - size % alignment;
		}

#pragma endregion

#pragma region Trivial & Nontrivial

#pragma region Construct

		template <typename T, typename... ARGUMENTS>
		constexpr static void construct_at(T* address, ARGUMENTS&&... arguments)
			noexcept(std::is_nothrow_constructible_v<T, ARGUMENTS...>)
		{
			std::construct_at<T>(address, std::forward<ARGUMENTS>(arguments)...);
		}

		template <Direction DIRECTION = Direction::Forward, bool DESTINATION_LEFT_INCLUSIVE = true, bool DESTROY_ON_THROW = true, std::forward_iterator DESTINATION_ITERATOR, typename... ARGUMENTS>
			requires (DIRECTION == Direction::Forward || std::bidirectional_iterator<DESTINATION_ITERATOR>)
		constexpr static void construct_range(DESTINATION_ITERATOR begin, DESTINATION_ITERATOR end, ARGUMENTS&&... arguments)
			noexcept (std::is_nothrow_constructible_v<std::iter_value_t<DESTINATION_ITERATOR>, ARGUMENTS...>
				&& (DIRECTION == Direction::Forward ? noexcept(++begin) : noexcept(--end)))
		{
			using T = std::iter_value_t<DESTINATION_ITERATOR>;

			if constexpr (std::contiguous_iterator<DESTINATION_ITERATOR> && std::is_trivially_constructible_v<T, ARGUMENTS...>) {
				if (begin != end) {
					T* raw_begin = std::to_address(begin) + !DESTINATION_LEFT_INCLUSIVE;
					T* raw_end = std::to_address(end) + !DESTINATION_LEFT_INCLUSIVE;
					repeat(raw_begin, raw_end, T(std::forward<ARGUMENTS>(arguments)...));
				}
			}
			else {
				if constexpr (DIRECTION == Direction::Forward) {
					DESTINATION_ITERATOR initial = begin;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_constructible_v<T, ARGUMENTS...>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = begin++;
							else
								destination = ++begin;

							construct_at(std::addressof(*destination), std::forward<ARGUMENTS>(arguments)...);
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = begin++;
								else
									destination = ++begin;

								construct_at(std::addressof(*destination), std::forward<ARGUMENTS>(arguments)...);
							}
						}
						catch (...) {
							destroy_range<Direction::Forward>(initial, begin);
							throw;
						}
					}
				}
				else {
					DESTINATION_ITERATOR initial = end;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_constructible_v<T, ARGUMENTS...>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = --end;
							else
								destination = end--;

							construct_at(std::addressof(*destination), std::forward<ARGUMENTS>(arguments)...);
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = --end;
								else
									destination = end--;

								construct_at(std::addressof(*destination), std::forward<ARGUMENTS>(arguments)...);
							}
						}
						catch (...) {
							destroy_range<Direction::Backward>(end, initial);
							throw;
						}
					}
				}
			}
		}

#pragma endregion

#pragma region Copy Construct

		template <typename T>
		constexpr static void copy_construct_at(T* destination, const T& source)
			noexcept(std::is_nothrow_copy_constructible_v<T>
				|| std::is_trivially_copy_constructible_v<T>)
		{
			construct_at(destination, source);
		}

		template <
			Direction DIRECTION = Direction::Forward,
			bool DESTINATION_LEFT_INCLUSIVE = true,
			bool SOURCE_LEFT_INCLUSIVE = true,
			bool DESTROY_ON_THROW = true,
			std::forward_iterator DESTINATION_ITERATOR,
			std::input_iterator SOURCE_ITERATOR
		>
			requires (DIRECTION == Direction::Forward || std::bidirectional_iterator<DESTINATION_ITERATOR>)
		constexpr static void copy_construct_range(DESTINATION_ITERATOR begin, DESTINATION_ITERATOR end, SOURCE_ITERATOR sourceBegin)
			noexcept (std::is_nothrow_copy_constructible_v<std::iter_value_t<DESTINATION_ITERATOR>>
				&& (DIRECTION == Direction::Forward ? noexcept(++begin) : noexcept(--end)))
		{
			using T = std::iter_value_t<DESTINATION_ITERATOR>;

			if constexpr (std::contiguous_iterator<DESTINATION_ITERATOR> && std::is_trivially_copy_constructible_v<T>) {
				if (begin != end)
					std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE), sourceBegin + (!SOURCE_LEFT_INCLUSIVE), (end - begin) * sizeof(T));
			}
			else {
				if constexpr (!SOURCE_LEFT_INCLUSIVE) {
					if (begin != end)
						++sourceBegin;
				}

				if constexpr (DIRECTION == Direction::Forward) {
					DESTINATION_ITERATOR initial = begin;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_copy_constructible_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = begin++;
							else
								destination = ++begin;

							copy_construct_at(std::addressof(*destination), *(sourceBegin++));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = begin++;
								else
									destination = ++begin;

								copy_construct_at(std::addressof(*destination), *(sourceBegin++));
							}
						}
						catch (...) {
							destroy_range<Direction::Forward>(initial, begin);
							throw;
						}
					}
				}
				else {
					DESTINATION_ITERATOR initial = end;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_copy_constructible_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = --end;
							else
								destination = end--;

							copy_construct_at(std::addressof(*destination), *(sourceBegin++));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = --end;
								else
									destination = end--;

								copy_construct_at(std::addressof(*destination), *(sourceBegin++));
							}
						}
						catch (...) {
							destroy_range<Direction::Backward>(end, initial);
							throw;
						}
					}
				}
			}
		}

#pragma endregion

#pragma region Destroy

		template <typename T>
		constexpr static void destroy_at(T* address)
			noexcept (std::is_nothrow_destructible_v<T>)
		{
			std::destroy_at(address);
		}

		template <
			Direction DIRECTION = Direction::Forward,
			bool LEFT_INCLUSIVE = true,
			bool DESTROY_ON_THROW = true,
			std::forward_iterator ITERATOR
		>
			requires (DIRECTION == Direction::Forward || std::bidirectional_iterator<ITERATOR>)
		constexpr static void destroy_range(ITERATOR begin, ITERATOR end)
			noexcept (std::is_nothrow_destructible_v<std::iter_value_t<ITERATOR>>
				&& (DIRECTION == Direction::Forward ? noexcept(++begin) : noexcept(--end)))
		{
			using T = std::iter_value_t<ITERATOR>;

			if constexpr (!std::is_trivially_destructible_v<T>) {
				if constexpr (DIRECTION == Direction::Forward) {
					ITERATOR initial = begin;
					if constexpr (!LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					while (begin != end) {
						ITERATOR destination;
						if constexpr (LEFT_INCLUSIVE)
							destination = begin++;
						else
							destination = ++begin;

						std::destroy_at(std::addressof(*destination));
					}
				}
				else {
					ITERATOR initial = end;
					if constexpr (!LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					while (begin != end) {
						ITERATOR destination;
						if constexpr (LEFT_INCLUSIVE)
							destination = --end;
						else
							destination = end--;

						std::destroy_at(std::addressof(*destination));
					}
				}
			}
		}

#pragma endregion

#pragma region Move Assign

		template <typename T>
		constexpr static void move_assign_at(T* destination, T&& source)
			noexcept (std::is_nothrow_move_assignable_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(destination, &source, sizeof(T));
			else
				*destination = std::move(source);
		}

		template <
			Direction DIRECTION = Direction::Forward,
			bool DESTINATION_LEFT_INCLUSIVE = true,
			bool SOURCE_LEFT_INCLUSIVE = true,
			bool DESTROY_ON_THROW = true,
			std::forward_iterator DESTINATION_ITERATOR,
			std::input_iterator SOURCE_ITERATOR
		>
			requires (DIRECTION == Direction::Forward || std::bidirectional_iterator<DESTINATION_ITERATOR>)
		constexpr static void move_assign_range(DESTINATION_ITERATOR begin, DESTINATION_ITERATOR end, SOURCE_ITERATOR sourceBegin)
			noexcept (std::is_nothrow_move_assignable_v<std::iter_value_t<DESTINATION_ITERATOR>>
				&& (DIRECTION == Direction::Forward ? noexcept(++begin) : noexcept(--end)))
		{
			using T = std::iter_value_t<DESTINATION_ITERATOR>;

			if constexpr (std::contiguous_iterator<DESTINATION_ITERATOR> && std::is_trivially_move_assignable_v<T>) {
				if (begin != end)
					std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE), sourceBegin + (!SOURCE_LEFT_INCLUSIVE), (end - begin) * sizeof(T));
			}
			else {
				if constexpr (!SOURCE_LEFT_INCLUSIVE) {
					if (begin != end)
						++sourceBegin;
				}

				if constexpr (DIRECTION == Direction::Forward) {
					DESTINATION_ITERATOR initial = begin;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_move_assignable_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = begin++;
							else
								destination = ++begin;

							move_assign_at(std::addressof(*destination), std::move(*(sourceBegin++)));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = begin++;
								else
									destination = ++begin;

								move_assign_at(std::addressof(*destination), std::move(*(sourceBegin++)));
							}
						}
						catch (...) {
							destroy_range<Direction::Forward>(initial, begin);
							throw;
						}
					}
				}
				else {
					DESTINATION_ITERATOR initial = end;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_move_assignable_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = --end;
							else
								destination = end--;

							move_assign_at(std::addressof(*destination), std::move(*(sourceBegin++)));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = --end;
								else
									destination = end--;

								move_assign_at(std::addressof(*destination), std::move(*(sourceBegin++)));
							}
						}
						catch (...) {
							destroy_range<Direction::Backward>(end, initial);
							throw;
						}
					}
				}
			}
		}

#pragma endregion

#pragma region Move Construct

		template <typename T>
		constexpr static void move_construct_at(T* destination, T&& source)
			noexcept (std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(destination, &source, sizeof(T));
			else
				construct_at(destination, std::move(source));
		}

		template <
			Direction DIRECTION = Direction::Forward,
			bool DESTINATION_LEFT_INCLUSIVE = true,
			bool SOURCE_LEFT_INCLUSIVE = true,
			bool DESTROY_ON_THROW = true,
			std::forward_iterator DESTINATION_ITERATOR,
			std::input_iterator SOURCE_ITERATOR
		>
			requires (DIRECTION == Direction::Forward || std::bidirectional_iterator<DESTINATION_ITERATOR>)
		constexpr static void move_construct_range(DESTINATION_ITERATOR begin, DESTINATION_ITERATOR end, SOURCE_ITERATOR sourceBegin)
			noexcept (std::is_nothrow_move_constructible_v<std::iter_value_t<DESTINATION_ITERATOR>>
				&& (DIRECTION == Direction::Forward ? noexcept(++begin) : noexcept(--end)))
		{
			using T = std::iter_value_t<DESTINATION_ITERATOR>;

			if constexpr (std::contiguous_iterator<DESTINATION_ITERATOR> && std::is_trivially_move_constructible_v<T>) {
				if (begin != end)
					std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE), sourceBegin + (!SOURCE_LEFT_INCLUSIVE), (end - begin) * sizeof(T));
			}
			else {
				if constexpr (!SOURCE_LEFT_INCLUSIVE) {
					if (begin != end)
						++sourceBegin;
				}

				if constexpr (DIRECTION == Direction::Forward) {
					DESTINATION_ITERATOR initial = begin;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_move_constructible_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = begin++;
							else
								destination = ++begin;

							move_construct_at(std::addressof(*destination), std::move(*(sourceBegin++)));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = begin++;
								else
									destination = ++begin;

								move_construct_at(std::addressof(*destination), std::move(*(sourceBegin++)));
							}
						}
						catch (...) {
							destroy_range<Direction::Forward>(initial, begin);
							throw;
						}
					}
				}
				else {
					DESTINATION_ITERATOR initial = end;
					if constexpr (!DESTINATION_LEFT_INCLUSIVE) {
						if (begin != end)
							++initial;
					}

					if constexpr (!DESTROY_ON_THROW || std::is_nothrow_move_constructible_v<T>) {
						while (begin != end) {
							DESTINATION_ITERATOR destination;
							if constexpr (DESTINATION_LEFT_INCLUSIVE)
								destination = --end;
							else
								destination = end--;

							move_construct_at(std::addressof(*destination), std::move(*(sourceBegin++)));
						}
					}
					else {
						try {
							while (begin != end) {
								DESTINATION_ITERATOR destination;
								if constexpr (DESTINATION_LEFT_INCLUSIVE)
									destination = --end;
								else
									destination = end--;

								move_construct_at(std::addressof(*destination), std::move(*(sourceBegin++)));
							}
						}
						catch (...) {
							destroy_range<Direction::Backward>(end, initial);
							throw;
						}
					}
				}
			}
		}

#pragma endregion

		template <typename T>
		constexpr static void reconstruct_at(T* destination, T&& source)
			noexcept (std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			move_construct_at(destination, std::move(source));
			destroy_at(&source);
		}

		template <typename T>
		constexpr static void replace_at(T* destination, T&& source)
			noexcept (std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			destroy_at(destination);
			move_construct_at(destination, std::move(source));
			destroy_at(&source);
		}

#pragma endregion

	}

}