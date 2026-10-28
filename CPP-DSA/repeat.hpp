#pragma once

#include <concepts>

namespace Algorithms {

	template <typename T>
	constexpr static void repeat(T* begin, T* end)
		noexcept
		requires std::is_trivially_copyable_v<T>
	{
		repeat(begin, static_cast<size_t>(end - begin));
	}

	template <typename T>
	constexpr static void repeat(T* begin, T* end, const T& value)
		noexcept
		requires std::is_trivially_copyable_v<T>
	{
		repeat(begin, static_cast<size_t>(end - begin), value);
	}

	template <typename T>
	constexpr static void repeat(T* destination, const size_t count)
		noexcept
		requires std::is_trivially_copyable_v<T>
	{
		auto* destination = static_cast<std::byte*>(destination);
		for (size_t written = sizeof(T); written < count * sizeof(T); written *= 2)
			std::memcpy(destination + written, destination, written);
	}

	template <typename T>
	constexpr static void repeat(T* destination, size_t count, const T& value)
		noexcept
		requires std::is_trivially_copyable_v<T>
	{
		std::memcpy(destination, &value, sizeof(T));
		repeat(destination, count);
	}

}