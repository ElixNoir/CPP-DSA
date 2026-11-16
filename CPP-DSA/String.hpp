#pragma once

#include "hash.hpp"

#include <iostream>

namespace DSA {

	class String {
	protected:

		char* data = nullptr;
		uint32_t size = 0;

	public:

#pragma region Constructors & Destructors

		constexpr String(char* data) noexcept : data(data), size(static_cast<uint32_t>(strlen(data))) {}

		constexpr String(char* data, uint32_t size) noexcept : data(data), size(size) {}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr const char* get_data() const noexcept {
			return data;
		}

		[[nodiscard]] constexpr uint32_t get_size() const noexcept {
			return size;
		}

		[[nodiscard]] constexpr auto hash() const noexcept {
			return Algorithms::hash_pointer(data);
		}

#pragma endregion

#pragma region Operators

		[[nodiscard]] constexpr char operator[](uint32_t index) const noexcept {
			return data[index];
		}

		[[nodiscard]] constexpr char& operator[](uint32_t index) noexcept {
			return data[index];
		}

#pragma region Comparison

		[[nodiscard]] constexpr bool operator!() const noexcept {
			return !data;
		}

#pragma region const char*

		[[nodiscard]] constexpr bool operator==(const char* other) const noexcept {
			return std::strncmp(data, other, size) == 0;
		}

		[[nodiscard]] constexpr bool operator!=(const char* other) const noexcept {
			return std::strncmp(data, other, size) != 0;
		}

		[[nodiscard]] constexpr bool operator>=(const char* other) const noexcept {
			return std::strncmp(data, other, size) >= 0;
		}

		[[nodiscard]] constexpr bool operator>(const char* other) const noexcept {
			return std::strncmp(data, other, size) > 0;
		}

		[[nodiscard]] constexpr bool operator<=(const char* other) const noexcept {
			return std::strncmp(data, other, size) <= 0;
		}

		[[nodiscard]] constexpr bool operator<(const char* other) const noexcept {
			return std::strncmp(data, other, size) < 0;
		}

#pragma endregion

#pragma region Self

		[[nodiscard]] constexpr bool operator==(String& other) const noexcept {
			return std::memcmp(data, other.data, size) == 0;
		}

		[[nodiscard]] constexpr bool operator!=(String& other) const noexcept {
			return std::memcmp(data, other.data, size) != 0;
		}

		[[nodiscard]] constexpr bool operator>=(String& other) const noexcept {
			return std::memcmp(data, other.data, size) >= 0;
		}

		[[nodiscard]] constexpr bool operator>(String& other) const noexcept {
			return std::memcmp(data, other.data, size) > 0;
		}

		[[nodiscard]] constexpr bool operator<=(String& other) const noexcept {
			return std::memcmp(data, other.data, size) <= 0;
		}

		[[nodiscard]] constexpr bool operator<(String& other) const noexcept {
			return std::memcmp(data, other.data, size) < 0;
		}

#pragma endregion

#pragma endregion

		friend std::ostream& operator<<(std::ostream& ostream, const String& t) {
			ostream << t.data;
			return ostream;
		}

#pragma endregion

#pragma endregion

	};

}