#pragma once

#include "Traits.hpp"

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

	template <typename T, typename... Arguments>
	constexpr void construct_range_backward(T* begin, const T* end, Arguments&&... arguments) noexcept(
		std::is_nothrow_constructible_v<T, Arguments...>
	) {
		if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
			repeat(begin, end, T(forward<Arguments>(arguments)...));
		else if constexpr (std::is_nothrow_constructible_v<T, Arguments...>) {
			while (begin != end)
				construct_at(--end, forward<Arguments>(arguments)...);
		}
		else {
			const T* const e = end;
			try {
				while (begin != end)
					construct_at(--end, forward<Arguments>(arguments)...);
			}
			catch (...) {
				destroy_range_backward(end, e);
				throw;
			}
		}
	}

	template <typename T, typename... Arguments>
	constexpr void construct_range_forward(T* begin, const T* end, Arguments&&... arguments) noexcept(
		std::is_nothrow_constructible_v<T, Arguments...>
	) {
		if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
			repeat(begin, end, T(forward<Arguments>(arguments)...));
		else if constexpr (std::is_nothrow_constructible_v<T, Arguments...>) {
			while (begin != end)
				construct_at(begin++, forward<Arguments>(arguments)...);
		}
		else {
			const T* const b = begin;
			try {
				while (begin != end)
					construct_at(begin++, forward<Arguments>(arguments)...);
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
		if constexpr (std::is_trivially_copy_constructible_v<T>)
			std::memcpy(destination, &source, sizeof(T));
		else
			construct_at(destination, source);
	}

	template <typename T>
	constexpr void copy_construct_range_backward(T* destination, const T* source, size_t count) noexcept(
		std::is_nothrow_copy_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copy_constructible_v<T>)
			std::memcpy(destination, source, count);
		else if constexpr (std::is_nothrow_copy_constructible_v<T>) {
			while (count-- != 0)
				copy_construct_at(destination + count, source[count]);
		}
		else {
			try {
				while (count-- != 0)
					copy_construct_at(destination + count, source[count]);
			}
			catch (...) {
				destroy_range_backward(destination + count, destination);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void copy_construct_range_forward(T* destination, const T* source, const size_t count) noexcept(
		std::is_nothrow_copy_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
		) {
		if constexpr (std::is_trivially_copy_constructible_v<T>)
			std::memcpy(destination, source, count);
		else {
			const T* const end = destination + count;
			if constexpr (std::is_nothrow_copy_constructible_v<T>) {
				while (destination != end)
					copy_construct_at(destination++, *(source++));
			}
			else {
				try {
					while (destination != end)
						copy_construct_at(destination++, *(source++));
				}
				catch (...) {
					destroy_range_forward(end - count, destination);
					throw;
				}
			}
		}
	}

	template <typename T>
	constexpr void destroy_at(T* address) noexcept {
		std::destroy_at(address);
	}

	template <typename T>
	constexpr void destroy_range_backward(T* begin, const T* end) noexcept {
		if constexpr (!std::is_trivially_destructible_v<T>)
			while (begin != end)
				destroy_at(--end);
	}

	template <typename T>
	constexpr void destroy_range_forward(T* begin, const T* end) noexcept {
		if constexpr (!std::is_trivially_destructible_v<T>)
			while (begin != end)
				destroy_at(begin++);
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
	constexpr void move_construct_at(T* destination, T& source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, &source, sizeof(T));
		else
			construct_at(destination, move(source));
	}

	template <typename T>
	constexpr void move_construct_range_backward(T* destination, T* source, size_t count) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, source, count);
		else if constexpr (std::is_nothrow_move_constructible_v<T>) {
			while (count-- != 0)
				move_construct_at(destination + count, source[count]);
		}
		else {
			try {
				while (count-- != 0)
					move_construct_at(destination + count, source[count]);
			}
			catch (...) {
				destroy_range_backward(destination + count, destination);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void move_construct_range_forward(T* destination, T* source, const size_t count) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, source, count);
		else {
			const T* const end = destination + count;
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				while (destination != end)
					move_construct_at(destination++, *(source++));
			}
			else {
				try {
					while (destination != end)
						move_construct_at(destination++, *(source++));
				}
				catch (...) {
					destroy_range_forward(end - count, destination);
					throw;
				}
			}
		}
	}

	template <typename T>
	constexpr void reconstruct_at(T* destination, T* source) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		move_construct_at(destination, source);
		destroy_at(source);
	}

	template <typename T>
	constexpr void reconstruct_range_backward(T* destination, T* source, size_t count) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, source, count);
		else if constexpr (std::is_nothrow_move_constructible_v<T>) {
			while (count-- != 0)
				reconstruct_at(destination + count, source + count);
		}
		else {
			try {
				while (count-- != 0)
					reconstruct_at(destination + count, source + count);
			}
			catch (...) {
				destroy_range_backward(destination + count, destination);
				throw;
			}
		}
	}

	template <typename T>
	constexpr void reconstruct_range_forward(T* destination, T* source, const size_t count) noexcept(
		std::is_nothrow_move_constructible_v<T>
		|| std::is_trivially_copyable_v<T>
	) {
		if constexpr (std::is_trivially_copyable_v<T>)
			std::memcpy(destination, source, count);
		else {
			const T* const end = destination + count;
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				while (destination != end)
					reconstruct_at(destination++, source++);
			}
			else {
				try {
					while (destination != end)
						reconstruct_at(destination++, source++);
				}
				catch (...) {
					destroy_range_forward(end - count, destination);
					throw;
				}
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