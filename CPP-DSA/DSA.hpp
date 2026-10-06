#pragma once

#include "Traits.hpp"

#include <memory>

namespace DSA {

#pragma region Functions

#pragma region Non-Trivial

	template <typename T, typename... Arguments>
	constexpr void construct_at(T* address, Arguments&&... arguments) {
		std::construct_at<T>(address, forward<Arguments>(arguments)...);
	}

	template <typename T, typename... Arguments>
	constexpr void construct_range_backward(T* begin, const T* end, Arguments&&... arguments) {
		if constexpr (!std::is_trivially_constructible_v<T>) {
			while (begin != end)
				construct_at(--end, arguments);
		}
		else
			repeat(begin, end, T(forward<Arguments>(arguments)...));
	}

	template <typename T, typename... Arguments>
	constexpr void construct_range_forward(T* begin, const T* end, Arguments&&... arguments) {
		if constexpr (!std::is_trivially_constructible_v<T>) {
			while (begin != end)
				construct_at(begin++, arguments);
		}
		else
			repeat(begin, end, T(forward<Arguments>(arguments)...));
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

	template <typename T, typename... Arguments>
	constexpr void move_construct_range_backward(T* destination, const T* source, size_t count) {
		if constexpr (!std::is_trivially_constructible_v<T>) {
			while (count-- != 0)
				construct_at(destination + count, move(source[count]));
		}
		else
			std::memcpy(destination, source, count);
	}

	template <typename T, typename... Arguments>
	constexpr void move_construct_range_forward(T* destination, const T* source, const size_t count) {
		if constexpr (!std::is_trivially_constructible_v<T>) {
			T* end = destination + count;
			while (destination != end)
				construct_at(destination++, move(*(source++)));
		}
		else
			std::memcpy(destination, source, count);
	}

	template <typename T>
	constexpr void reconstruct_at(T* destination, T* source) {
		construct_at(destination, move(*source));
		destroy_at(source);
	}

	template <typename T>
	constexpr void reconstruct_range_backward(T* destination, T* source, size_t count) {
		if constexpr (!std::is_trivially_destructible_v<T>)
			while (count-- != 0)
				reconstruct_at(destination + count, source + count);
	}

	template <typename T>
	constexpr void reconstruct_range_forward(T* destination, T* source, const size_t count) {
		if constexpr (!std::is_trivially_destructible_v<T>) {
			T* end = destination + count;
			while (destination != end)
				reconstruct_at(destination++, source++);
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