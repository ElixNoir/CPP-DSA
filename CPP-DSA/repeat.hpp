#pragma once

#include <concepts>
#include <cstring>

namespace Algorithms {

	template <typename T>
	constexpr static void repeat(T* const begin, const T* const end) noexcept
		requires std::is_trivially_copyable_v<T>
	{
		repeat(begin, static_cast<size_t>(end - begin));
	}

	template <typename T>
	constexpr static void repeat(T* const begin, const T* const end, const T& data) noexcept
		requires std::is_trivially_copyable_v<T>
	{
		repeat(begin, static_cast<size_t>(end - begin), data);
	}

	template <typename T>
	constexpr static void repeat(T* const destination, const size_t count) noexcept
		requires std::is_trivially_copyable_v<T>
	{
		auto* destination = static_cast<std::byte*>(destination);
		for (size_t written = sizeof(T); written < count * sizeof(T); written <<= 1)
			std::memcpy(destination + written, destination, written);
	}

	template <typename T>
	constexpr static void repeat(T* const destination, const size_t count, const T& data) noexcept
		requires std::is_trivially_copyable_v<T>
	{
		std::memcpy(destination, &data, sizeof(T));
		repeat(destination, count);
	}

}