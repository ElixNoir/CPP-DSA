#pragma once

#include "Bitwise.hpp"
#include "../Structures/Hashable.hpp"
#include "../Utilities/Traits.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace Algorithms {

	template <DSA::concepts::Hashable T>
	[[nodiscard]] constexpr static auto hash(const T& value)
		noexcept (noexcept(value.hash()))
	{
		return value.hash();
	}

	template <typename T>
	[[nodiscard]] constexpr static auto hash_pointer(const T* value, const uintmax_t seed = 0) noexcept {
		constexpr size_t chunkSize = sizeof(uintmax_t);
		uintmax_t hash = seed;
		if constexpr (constexpr size_t size = sizeof(T); Bitwise::is_multiple_of_power_of_2(size, chunkSize)) {
			const std::byte* begin = value;
			const std::byte* end = begin + size;
			while (begin < end) {
				hash ^= *reinterpret_cast<const uintmax_t*>(begin);
				begin += chunkSize;
			}
		}
		else {
			const std::byte* cursor = value;
			size_t bytes_left = size;
			while (bytes_left >= chunkSize) {
				hash ^= *reinterpret_cast<const uintmax_t*>(cursor);
				cursor += chunkSize;
				bytes_left -= chunkSize;
			}

			if (bytes_left > 0) {
				uintmax_t chunk = 0;
				std::memcpy(&chunk, cursor, bytes_left);
				hash ^= chunk;
			}
		}

		return hash;
	}

	template <typename T>
	[[nodiscard]] constexpr static auto hash(const T& value) noexcept {
		if constexpr (std::is_pointer_v<T>)
			return hash_pointer(value);
		else {
			if constexpr (constexpr size_t size = sizeof(value); size == 1)
				return std::bit_cast<uint8_t>(value);
			else if constexpr (size == 2)
				return std::bit_cast<uint16_t>(value);
			else if constexpr (size == 4)
				return std::bit_cast<uint32_t>(value);
			else if constexpr (size == 8)
				return std::bit_cast<uint64_t>(value);
			else
				return hash_pointer(reinterpret_cast_single_pointer(value));
		}
	};

	template <typename T>
	[[nodiscard]] constexpr static auto hash(const T& value, uintmax_t seed) noexcept {
		if constexpr (std::is_pointer_v<T>)
			return hash_pointer(value, seed);
		else {
			if constexpr (constexpr size_t size = sizeof(value); size == 1)
				return seed ^ std::bit_cast<uint8_t>(value);
			else if constexpr (size == 2)
				return seed ^ std::bit_cast<uint16_t>(value);
			else if constexpr (size == 4)
				return seed ^ std::bit_cast<uint32_t>(value);
			else if constexpr (size == 8)
				return seed ^ std::bit_cast<uint64_t>(value);
			else
				return hash_pointer(reinterpret_cast_single_pointer(value), seed);
		}
	};

}