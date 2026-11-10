#pragma once

#include "Bitwise.hpp"
#include "Hashable.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace Algorithms {

	template <DSA::concepts::Hashable T>
	[[nodiscard]] constexpr auto hash(T value)
		noexcept (noexcept(value.hash()))
	{
		return value.hash();
	}

	template <typename T>
	[[nodiscard]] constexpr T hash(T value) noexcept {
		constexpr size_t size = sizeof(value);
		constexpr size_t chunkSize = sizeof(uintmax_t);
		if constexpr (size == 1)
			return std::bit_cast<uint8_t>(value);
		else if constexpr (size == 2)
			return std::bit_cast<uint16_t>(value);
		else if constexpr (size == 4)
			return std::bit_cast<uint32_t>(value);
		else if constexpr (size == 8)
			return std::bit_cast<uint64_t>(value);
		else if constexpr (Bitwise::is_multiple_of_power_of_2(size, chunkSize)) {
			uintmax_t hash = 0;

			const std::byte* begin = reinterpret_cast<const std::byte*>(&value);
			const std::byte* end = begin + size;
			while (begin < end) {
				hash ^= *reinterpret_cast<uintmax_t*>(begin);
				begin += chunkSize;
			}

			return hash;
		}
		else {
			uintmax_t hash = 0;

			const std::byte* cursor = reinterpret_cast<const std::byte*>(&value);
			size_t bytes_left = size;
			while (bytes_left >= chunkSize) {
				hash ^= *reinterpret_cast<uintmax_t*>(cursor);
				cursor += chunkSize;
				bytes_left -= chunkSize;
			}

			if (bytes_left > 0) {
				uintmax_t chunk = 0;
				std::memcpy(&chunk, cursor, bytes_left);
				hash ^= chunk;
			}
		}
	};

}