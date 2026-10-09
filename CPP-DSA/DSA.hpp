#pragma once

#include "Traits.hpp"
#include "Utilities.hpp"

#include <memory>

namespace DSA {

#pragma region Functions

#pragma region Non-Trivial

	template <typename T, typename... Arguments>
	constexpr void construct_at(T* address, Arguments&&... arguments) noexcept(
		std::is_nothrow_constructible_v<T, Arguments...>
	) {
		std::construct_at<T>(address, forward<Arguments>(arguments)...);
	}

	template <bool LEFT_INCLUSIVE = true, typename T, typename... Arguments>
	constexpr void construct_range_backward(T* begin, const T* end, Arguments&&... arguments) noexcept(
		std::is_nothrow_constructible_v<T, Arguments...>
	) {
		if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
			repeat(begin + !LEFT_INCLUSIVE, end + !LEFT_INCLUSIVE, T(forward<Arguments>(arguments)...));
		else {
			const T* const e = end + !LEFT_INCLUSIVE;
			try {
				while (begin != end)
					construct_at(LEFT_INCLUSIVE ? --end : end--, forward<Arguments>(arguments)...);
			}
			catch (...) {
				destroy_range_backward(end, e);
				throw;
			}
		}
	}

	template <bool LEFT_INCLUSIVE = true, typename T, typename... Arguments>
	constexpr void construct_range_forward(T* begin, const T* end, Arguments&&... arguments) noexcept(
		std::is_nothrow_constructible_v<T, Arguments...>
	) {
		if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
			repeat(begin + !LEFT_INCLUSIVE, end + !LEFT_INCLUSIVE, T(forward<Arguments>(arguments)...));
		else {
			const T* const b = begin + !LEFT_INCLUSIVE;
			try {
				while (begin != end)
					construct_at(LEFT_INCLUSIVE ? begin++ : ++begin, forward<Arguments>(arguments)...);
			}
			catch (...) {
				destroy_range_forward(b, begin);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void copy_construct_at(T* destination, const T& source) noexcept(
		std::is_nothrow_copy_constructible_v<T>
		|| std::is_trivially_copy_constructible_v<T>
	) {
		construct_at(destination, source);
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void copy_construct_range_backward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_copy_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copy_constructible_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
			try {
				while (begin != end)
					copy_construct_at(
						DESTINATION_LEFT_INCLUSIVE ? --end : end--,
						*(SOURCE_LEFT_INCLUSIVE ? --source : source--));
			}
			catch (...) {
				destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
				throw;
			}
		}
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void copy_construct_range_forward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_copy_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copy_constructible_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
			try {
				while (begin != end)
					copy_construct_at(
						DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
						*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source));
			}
			catch (...) {
				destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void destroy_at(T* address) noexcept {
		std::destroy_at(address);
	}

	template <bool LEFT_INCLUSIVE = true, typename T>
	constexpr void destroy_range_backward(T* begin, const T* end) noexcept {
		if constexpr (!std::is_trivially_destructible_v<T>)
			while (begin != end)
				destroy_at(LEFT_INCLUSIVE ? --end : end--);
	}

	template <bool LEFT_INCLUSIVE = true, typename T>
	constexpr void destroy_range_forward(T* begin, const T* end) noexcept {
		if constexpr (!std::is_trivially_destructible_v<T>)
			while (begin != end)
				destroy_at(LEFT_INCLUSIVE ? begin++ : ++begin);
	}

	template <typename T>
	constexpr T&& forward(std::remove_reference_t<T>& data) {
		return static_cast<T&&>(data);
	}

	template <typename T>
	constexpr std::remove_reference_t<T>&& move(T&& data) {
		return std::move(data);
	}

	template <typename T>
	constexpr void move_construct_at(T* destination, T&& source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, &source, sizeof(T));
		else
			construct_at(destination, move(source));
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void move_construct_range_backward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
			try {
				while (begin != end)
					move_construct_at(
						DESTINATION_LEFT_INCLUSIVE ? --end : end--,
						move(*(SOURCE_LEFT_INCLUSIVE ? --source : source--)));
			}
			catch (...) {
				destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
				throw;
			}
		}
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void move_construct_range_forward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
			try {
				while (begin != end)
					move_construct_at(
						DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
						move(*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source)));
			}
			catch (...) {
				destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void reconstruct_at(T* destination, T&& source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		move_construct_at(destination, move(source));
		destroy_at(&source);
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void reconstruct_range_backward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
			try {
				source += end - begin;
				while (begin != end)
					reconstruct_at(
						DESTINATION_LEFT_INCLUSIVE ? --end : end--,
						move(*(SOURCE_LEFT_INCLUSIVE ? --source : source--)));
			}
			catch (...) {
				destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
				throw;
			}
		}
	}

	template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
	constexpr void reconstruct_range_forward(T* begin, const T* end, T* source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
		else {
			const T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
			try {
				while (begin != end)
					reconstruct_at(
						DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
						move(*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source)));
			}
			catch (...) {
				destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
				throw;
			}
		}
	}

#pragma endregion

#pragma region Trivial

	template <typename T>
	constexpr void repeat(T* begin, const T* end) noexcept requires std::is_trivially_copyable_v<T> {
		repeat(begin, static_cast<size_t>(end - begin));
	}

	template <typename T>
	constexpr void repeat(T* begin, const T* end, const T& value) noexcept requires std::is_trivially_copyable_v<T> {
		repeat(begin, static_cast<size_t>(end - begin), value);
	}

	template <typename T>
	constexpr void repeat(T* destination, const size_t count) noexcept requires std::is_trivially_copyable_v<T> {
		auto* destination = static_cast<std::byte*>(destination);
		for (size_t written = sizeof(T); written < count * sizeof(T); written *= 2)
			std::memcpy(destination + written, destination, written);
	}

	template <typename T>
	constexpr void repeat(T* destination, size_t count, const T& value) noexcept requires std::is_trivially_copyable_v<T> {
		std::memcpy(destination, &value, sizeof(T));
		repeat(destination, count);
	}

#pragma endregion

#pragma endregion

}